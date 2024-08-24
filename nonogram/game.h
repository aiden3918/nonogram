#pragma once

#ifndef game_h
#define game_h

#include "util.h"
#include <random>
#include <sstream>
#include "olcPixelGameEngine.h"
#include "olcPGEX_QuickGUI.h"

const enum class TileState {EMPTY = 0, FILLED = 1, FLAGGED = 2};

struct GameTile {
	vec2D pos;
	AABB hitbox;
	int tSize;
	bool active;
	int r; // "y" value
	int c; // "x" value
	TileState state;

	GameTile();
	GameTile(const vec2D& gridPos, const int& tileSize, const int& row, const int& column);
	~GameTile();
};

struct GameGrid {
	// 5x5, 10x10, 15x15, 20x20
	std::vector<GameTile> tiles;
	std::vector<std::pair<std::vector<int>, std::string>> _horizontalHints; // sides that give hints (element 0 is horizntal, element 1 is vertical)
	std::vector<std::pair<std::vector<int>, std::string>> _verticalHints; // sides that give hints (element 0 is horizntal, element 1 is vertical)
	std::vector<std::vector<bool>> answer;

	vec2D pos; // top-left of where clickable tiles starts
	int tSize;

	// init with tiles
	void Clear(const int& numTiles, const int& tileSize = 5);

	// randomize board
	void CreateNewAnswer();

	void CreateBoard();
};

class Game {
public:
	Game();
	~Game();

	void NewGame(int numTiles = 5, int tileSize = 5);
	void Update(olc::PixelGameEngine* engine);
	void Draw(olc::PixelGameEngine* engine);

	GameGrid gameGrid;
private:
	bool _mouseDown;
	TileState _mouseActiveState;
	
	bool _matchesAnswer();
	
};

#endif