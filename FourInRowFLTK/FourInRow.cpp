#include "FourInRow.h"

FourInRow::FourInRow() :
	current_state(GameState::TurnWhite),
	taken_space(0),
	total_cells(TABLE_ROWS*TABLE_COLS)
{
	for (size_t y = 0; y < TABLE_ROWS; y++)
		for (size_t x = 0; x < TABLE_COLS; x++)
			cells[y][x] = Color::None;
}

FourInRow::~FourInRow() {}

bool FourInRow::isClickableCell(unsigned int x, unsigned int y) const
{
	if (cells[y][x] != Color::None)
		return false;
	if (y + 1 < TABLE_ROWS)
		return cells[y + 1][x] != Color::None;
	return true;
}

void FourInRow::makeTurn(unsigned int x, unsigned int y)
{
	if (current_state != GameState::TurnBlack &&
		current_state != GameState::TurnWhite)
		return;
	if (!isClickableCell(x, y))
		return;
	cells[y][x] = (current_state == GameState::TurnBlack) ?
		Color::Black : Color::White;
	taken_space++;
	Color winner = hasWon(x, y, cells[y][x]);
	if (winner == Color::White)
	{
		current_state = GameState::WinWhite;
		return;
	}
	else if (winner == Color::Black)
	{
		current_state = GameState::WinBlack;
		return;
	}
	if (taken_space == total_cells)
	{
		current_state = GameState::Tie;
		return;
	}
	current_state = (current_state == GameState::TurnBlack) ?
		GameState::TurnWhite : GameState::TurnBlack;
}

Color FourInRow::hasWon(unsigned int x, unsigned int y, Color current) const
{
	int h = 1;
	//horizontal right
	for (size_t i = x + 1; i < TABLE_COLS &&
		cells[y][i] == current; i++)
		h++;
	//horizontal left
	for (size_t i = x - 1; i >= 0 &&
		cells[y][i] == current; i--)
		h++;
	if (h >= 4) return current;

	int v = 1;
	//vertical lower
	for (size_t i = y + 1; i < TABLE_ROWS &&
		cells[i][x] == current; i++)
		v++;
	//vertical upper
	for (size_t i = y - 1; i >= 0 &&
		cells[i][x] == current; i--)
		v++;
	if (v >= 4) return current;

	int diagl = 1;
	//diag \ lower
	for (size_t i = x + 1, j = y + 1;
		i < TABLE_COLS&&j < TABLE_ROWS&&
		cells[j][i] == current; i++, j++)
		diagl++;
	//diag \ upper
	for (size_t i = x - 1, j = y - 1;
		i >= 0 && j >= 0 &&
		cells[j][i] == current; i--, j--)
		diagl++;
	if (diagl >= 4) return current;

	int diagr = 1;
	//diag / lower
	for (size_t i = x - 1, j = y + 1;
		i >= 0 && j < TABLE_ROWS&&
		cells[j][i] == current; i--, j++)
		diagr++;
	//diag / upper
	for (size_t i = x + 1, j = y - 1;
		i >= TABLE_COLS && j >= 0 &&
		cells[j][i] == current; i++, j--)
		diagr++;
	if (diagr >= 4) return current;


	return Color::None;
}
