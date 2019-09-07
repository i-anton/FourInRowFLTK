#pragma once
#include "stdafx.h"
#include <vector>

enum class GameState
{
	TurnBlack,
	TurnWhite,
	WinBlack,
	WinWhite,
	Tie
};
enum class Color
{
	None, White, Black
};

class FourInRow
{
public:
	FourInRow();
	~FourInRow();
	GameState getState() const { return current_state; }
	Color getCell(unsigned int x, unsigned int y) const { return cells[y][x]; }
	bool isClickableCell(unsigned int x, unsigned int y) const;
	void makeTurn(unsigned int x, unsigned int y);
private:
	Color hasWon(unsigned int x, unsigned int y, Color current) const;
private:
	GameState current_state;
	Color cells[TABLE_ROWS][TABLE_COLS];
	unsigned int taken_space;
	const unsigned int total_cells;
};

