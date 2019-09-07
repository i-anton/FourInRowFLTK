#pragma once
#define WIN32
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <vector>
#include <iostream>
#include <FL/Fl.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_Box.H>
#include "strings.h"

constexpr int button_width = 32;
constexpr int TABLE_ROWS = 6;
constexpr int TABLE_COLS =8;
#define MIN(x,y) ((x)>(y))?(y):(x)
#define MAX(x,y) ((x)>(y))?(x):(y)