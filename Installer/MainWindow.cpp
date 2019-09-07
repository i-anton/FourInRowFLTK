#include "MainWindow.h"
#include "InstallationWindow.h"
#include <FL/fl_ask.H>

MainWindow::MainWindow() :
	Fl_Window(window_width, window_height, STR_WINDOW_TITLE),
	drives(systemDrivesInfo()),
	drive(nullptr)
{
	this->label();
	srand(static_cast<unsigned int>(time(0)));
	this->begin();
	title = std::unique_ptr<Fl_Box>(
		new Fl_Box(FL_FRAME_BOX, gui_offset, gui_offset,
			window_width - 2 * gui_offset,
			button_height, STR_WINDOW_TITLE));
	title->color(FL_GRAY - 4);
	title->labelsize(20);
	title->labelfont(FL_BOLD);
	title->labeltype(FL_ENGRAVED_LABEL);
	start_install = std::unique_ptr<Fl_Button>(
		new Fl_Button(window_width - gui_offset - button_width,
			window_height - gui_offset - button_height,
			button_width,
			button_height, STR_DONE));
	start_install->shortcut(FL_Enter);
	start_install->callback(install_cb,
		(void*)this);
	start_install->deactivate();
	install_dest = std::unique_ptr<Fl_Output>(
		new Fl_Output(6 * gui_offset,
			button_height + 2 * gui_offset,
			window_width - 8 * gui_offset - button_width,
			button_height, STR_INSTALL_PATH));
	select_install_dest = std::unique_ptr<Fl_Button>(
		new Fl_Button(window_width - gui_offset - button_width,
			button_height + 2 * gui_offset,
			button_width,
			button_height, STR_CHANGE_PATH));
	select_install_dest->shortcut(FL_Enter);
	select_install_dest->callback(open_cb,
		(void*)this);
	choice = std::unique_ptr<Fl_Choice>(new Fl_Choice(3 * gui_offset + button_width,
		2 * button_height + 3 * gui_offset,
		window_width - 4 * gui_offset - button_width,
		button_height, u8"Bind to storage: "));
	this->end();
	updateDrivesChoice();
}
void MainWindow::quit_cb(Fl_Widget *, void *)
{
	exit(0);
}

void MainWindow::open_cb(Fl_Widget *, void * data)
{
	Fl_Native_File_Chooser fc;
	fc.title(u8"Installation path");
	fc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	int result = fc.show();
	if (result == 0)
	{
		static_cast<MainWindow*>(data)->update_path(fc.filename());
	}
	else if (result == -1)
	{
		fl_alert(fc.errmsg());
	}
}

void MainWindow::drivechange_cb(Fl_Widget *, void *data)
{
	DriveChangedEvent* ev = static_cast<DriveChangedEvent *>(data);
	ev->window->drive = &(ev->window->drives[ev->drvIdx]);
	std::cout << ev->drvIdx;
	if (ev->window->drive != nullptr &&ev->window->install_path.length() != 0)
		ev->window->start_install->activate();
}

void MainWindow::install_cb(Fl_Widget *, void *data)
{
	auto wnd = static_cast<MainWindow*>(data);
	wnd->hide();
	auto install = std::unique_ptr<InstallationWindow>(
		new InstallationWindow(wnd->install_path,wnd->drive));
	install->show();
	while (install->shown()) Fl::wait();
}

void MainWindow::update_path(std::string new_path)
{
	install_path = new_path.c_str();
	install_dest->value(new_path.c_str());
	if (drive != nullptr && install_path.length() != 0)
		start_install->activate();
}

std::vector<DriveEntry> MainWindow::systemDrivesInfo()
{
	std::vector<DriveEntry> drives;
	DWORD drivesByte = GetLogicalDrives();
	for (short i = 0; i < 26; i++)
	{
		if (!(drivesByte&(1 << i)))continue;

		char letter = 'A' + i;
		std::cout << letter;
		std::string strLetterPath;
		strLetterPath += letter;
		strLetterPath.append(":\\");
		UINT typeMask = GetDriveType(strLetterPath.c_str());
		std::cout << typeMask;
		if (typeMask == DRIVE_FIXED || typeMask == DRIVE_REMOVABLE)
		{
			drives.push_back(DriveEntry{ letter,typeMask,strLetterPath });
		}
		std::cout << std::endl;
	}
	for (size_t i = 0; i < drives.size(); i++)
	{
		DriveEntry drv = drives[i];
		TCHAR volumeName[MAX_PATH + 1] = { 0 };
		TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
		DWORD serialNumber = 0;
		DWORD maxComponentLen = 0;
		DWORD fileSystemFlags = 0;

		if (!GetVolumeInformationA(drv.strPath.c_str(),
			volumeName, MAX_PATH, &serialNumber, &maxComponentLen,
			&fileSystemFlags, fileSystemName, MAX_PATH))
		{
			drives.clear();
			return drives;
		}

		drives[i].volName = std::string(volumeName);
		drives[i].fsName = std::string(fileSystemName);
		drives[i].serial = serialNumber;
	}
	return drives;
}

void MainWindow::updateDrivesChoice()
{
	for (size_t i = 0; i < drives.size(); i++)
	{
		auto drive = drives[i];
		auto num = i;
		choice->add(drive.strPath.c_str(),
			FL_ALT | tolower(drive.letter),
			drivechange_cb, (void*)(new DriveChangedEvent{ num,this })
		);
	}
}

