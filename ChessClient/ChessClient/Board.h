#pragma once
#include "Tile.h"

class Board
{
public:
	Board();
	Board(int startX, int startY, int width);
	void InitBoard(PlayerType::Player player, PlayerType::Player opponent);
	Tile* FindTile(int x, int y);
	int _startX;
	int _startY;
	int _width;
	Tile board[8][8];
};