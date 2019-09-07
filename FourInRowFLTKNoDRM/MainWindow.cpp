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
	model(nullptr)
{
	this->label(S_MAIN_WINDOW);
	srand(static_cast<unsigned int>(time(0)));
	this->begin();
	initFieldGUI();
	buildMenu();
	this->end();
	resetModel();
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
	model->makeTurn(x, y);
	auto st = model->getState();
	updateWidgets();
	if (st == GameState::Tie)
		fl_alert(S_TIE_GAME);
	else if (st == GameState::WinBlack)
		fl_alert(S_BLACK_WON_GAME);
	else if (st == GameState::WinWhite)
		fl_alert(S_WHITE_WON_GAME);
}
