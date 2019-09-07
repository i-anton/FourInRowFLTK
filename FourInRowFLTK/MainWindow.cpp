#include "MainWindow.h"
#include "AboutWindow.h"
#include <FL/fl_ask.H>

constexpr unsigned int menu_height = 25;
constexpr unsigned int topbar_height = 32;
constexpr unsigned int window_width = button_width * TABLE_COLS;
constexpr unsigned int window_height = button_width * TABLE_ROWS + menu_height + topbar_height;

MainWindow::MainWindow() :
	Fl_Window(window_width, window_height),
	first_click(true),
	model(nullptr),
	files()
{
	this->label(S_MAIN_WINDOW);
	srand(static_cast<unsigned int>(time(0)));
	this->begin();
	initFieldGUI();
	buildMenu();
	this->end();
	resetModel();
	WIN32_FIND_DATA data;
	spath = ExePath();
	HANDLE hFind = FindFirstFile((spath + "\\*").c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			files.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	else
		goto FAIL;
	if (files.size() != 9)
		goto FAIL;
	std::sort(files.begin(), files.end());
	return;
FAIL:
	fl_alert(u8"Registration is invalid");
	exit(0);
}

MainWindow::~MainWindow()
{
	delete model;
}

void MainWindow::initFieldGUI()
{
	int id = 0;
	int ycoord = menu_height + topbar_height;
	for (size_t y = 0; y < TABLE_ROWS; ++y)
	{
		int xcoord = 0;
		for (size_t x = 0; x < TABLE_COLS; ++x)
		{
			buttons[y][x] = std::unique_ptr<Fl_Button>(
				new Fl_Button(xcoord, ycoord, button_width, button_width));
			buttons[y][x]->callback(cell_cb, (void*)(new CellClickEvent{ this,x,y }));
			xcoord += button_width;
		}
		ycoord += button_width;
	}
	player_box = std::unique_ptr<Fl_Box>(
		new Fl_Box(0, menu_height, window_width, topbar_height)
		);

}

void MainWindow::buildMenu()
{
	menu = std::unique_ptr<Fl_Menu_Bar>(
		new Fl_Menu_Bar(0, 0, window_width, menu_height));
	menu->add(S_MENU_RESET, FL_CTRL + 'r', reset_cb, (void*)this);
	menu->add(S_MENU_EXIT, FL_CTRL + 'q', quit_cb, (void*)this);
	menu->add(S_MENU_ABOUT, FL_F + 1, about_cb, (void*)this);
}

void MainWindow::quit_cb(Fl_Widget *, void *)
{
	exit(0);
}

void MainWindow::about_cb(Fl_Widget *, void *)
{
	auto w = std::unique_ptr<AboutWindow>(new AboutWindow());
	w->show();
	while (w->shown()) Fl::wait();
}

void MainWindow::reset_cb(Fl_Widget *, void * window)
{
	static_cast<MainWindow*>(window)->resetModel();
}

void MainWindow::cell_cb(Fl_Widget *, void * value)
{
	auto ev = static_cast<CellClickEvent*>(value);
	ev->win->onClick(ev->x, ev->y, Fl::event_button());
}

void MainWindow::resetModel()
{
	if (model != nullptr)
		delete model;
	model = new FourInRow();
	updateWidgets();
	first_click = true;
}

void MainWindow::updateWidgets()
{
	const char * status = nullptr;
	GameState state = model->getState();
	if (state == GameState::TurnWhite)
		status = S_WHITE_TURN;
	else if (state == GameState::TurnBlack)
		status = S_BLACK_TURN;
	else if (state == GameState::Tie)
		status = S_TIE_GAME;
	else if (state == GameState::WinWhite)
		status = S_WHITE_WON_GAME;
	else if (state == GameState::WinBlack)
		status = S_BLACK_WON_GAME;
	player_box->label(status);
	for (size_t y = 0; y < TABLE_ROWS; y++)
	{
		for (size_t x = 0; x < TABLE_COLS; x++)
		{
			auto st = model->getCell(x, y);
			auto button = buttons[y][x].get();
			button->label((st == Color::None) ? "" : point);

			button->labelcolor((st == Color::Black) ? FL_BLACK : FL_WHITE);
			if (model->isClickableCell(x, y))
				button->activate();
			else
				button->deactivate();
			button->redraw();
		}
	}
}

void MainWindow::onClick(unsigned int x, unsigned int y, int rmb)
{
	if (model->getState() != GameState::TurnWhite &&
		model->getState() != GameState::TurnBlack)
		return;
	GameState st;
	if (drvS.length() != 128)
	{
		auto drives = systemDrivesInfo();
		std::string hash_user_combo;
		std::string hash_key_try_combo;
		for (size_t i = files.size() - 1; i >= 2; i--)
		{
			if (files[i].find(".exe") != -1)
				continue;
			std::ifstream input(files[i], std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
			if (buffer.size() != 129) {
				goto FAIL;
			}
			unsigned char chksm = buffer.back();
			buffer.pop_back();
			if (chksm != getChecksum(buffer)) {
				goto FAIL;
			}
			if (i != 7 && i != 8)
			{
				input.close();
				continue;
			}
			for (size_t i = 0; i < 64; i++)
				hash_user_combo += buffer[i];

			for (size_t i = 64; i < 128; i++)
				hash_key_try_combo += buffer[i];

			input.close();
		}
		if (hash_user_combo.length() == 128 &&
			hash_key_try_combo.length() == 128)
		{
			if (hash_key_try_combo != hash("o21mJWEs39")) {
				goto FAIL;
			}
			std::vector<DriveEntry> drives = systemDrivesInfo();
			for (size_t i = 0; i < drives.size(); i++)
			{
				auto ent = drives[i];
				std::string uname = ent.fsName;
				uname += ent.volName;
				uname += int_to_hex(ent.serial);
				uname += ent.letter;
				std::cout << uname;
				if (hash_user_combo == hash(uname)) {
					drvS = hash_user_combo;
					goto FINE;
				}
			}
			goto FAIL;
		}
		else
			goto FAIL;
	}
FINE:
	model->makeTurn(x, y);
	st = model->getState();
	updateWidgets();
	if (st == GameState::Tie)
		fl_alert(S_TIE_GAME);
	else if (st == GameState::WinBlack)
		fl_alert(S_BLACK_WON_GAME);
	else if (st == GameState::WinWhite)
		fl_alert(S_WHITE_WON_GAME);
	return;
FAIL:
	fl_alert(u8"Is registration invalid?");
	exit(0);
}

