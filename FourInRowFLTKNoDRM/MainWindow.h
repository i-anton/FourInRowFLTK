#pragma once
#include "stdafx.h"
#include "FourInRow.h"
#include <FL/Fl_Menu_Bar.H>

class MainWindow : public Fl_Window
{
public:
	MainWindow();
	~MainWindow();
private:
	void initFieldGUI();
	void buildMenu();
	static void quit_cb(Fl_Widget*,void*);
	static void about_cb(Fl_Widget*, void*);
	static void reset_cb(Fl_Widget*, void*);
	static void cell_cb(Fl_Widget*, void*);
private:
	void resetModel();
	void updateWidgets();
	void onClick(unsigned int x, unsigned int y,int rmb);
private:
	std::unique_ptr<Fl_Menu_Bar> menu;
	std::unique_ptr<Fl_Box> player_box;
	std::unique_ptr<Fl_Button> buttons[TABLE_ROWS][TABLE_COLS];
	FourInRow* model;
	bool first_click;
	const char * point = u8"@circle";
};

struct CellClickEvent {
	MainWindow* win;
	size_t x, y;
};
