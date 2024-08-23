#pragma once

#ifndef game_h
#define game_h

#include "util.h"
#include "olcPixelGameEngine.h"
#include "olcPGEX_QuickGUI.h"

struct GameTile {
	vec2D pos;
	int tileSize;
	bool active;
};

struct GameGrid {
	// 5x5, 10x10, 15x15, 20x20
	std::vector<GameTile> tiles;
	std::vector<int>* _sideHints[2]; // sides that give hints
	std::vector<std::vector<bool>> answer;

	vec2D gameTilesPos; // top-left of where clickable tiles starts
	int tileSize;
};

class Game {
public:
	Game();
	~Game();

private:
	GameGrid _gameGrid;
	
};

#endif