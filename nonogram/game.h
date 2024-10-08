#pragma once

#ifndef game_h
#define game_h

#include "util.h"
#include <random>
#include <sstream>
#include "olcPixelGameEngine.h"
#include "olcPGEX_QuickGUI.h"

const enum class TileState {EMPTY = 0, FILLED = 1, FLAGGED = 2};
const enum class GameState {PLAYING, WIN};

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

struct HintData {
	std::vector<int> vec;
	std::string str;
	bool correct;
};

struct GameGrid {
	// 5x5, 10x10, 15x15, 20x20
	std::vector<GameTile> tiles;
	// sides that give hints (element 0 is horizntal, element 1 is vertical)
	std::vector<HintData> horizontalHints, verticalHints;
	std::vector<std::vector<bool>> answer;
	int horiDisp, vertDisp;
	AABB gameTilesDimensions;

	vec2D pos; // top-left of where clickable tiles starts
	int tSize;
	vec2D center;
	vec2D gridSize;

	// init with tiles
	void Clear(const vec2D& screenCenter, const int& numTiles, const int& tileSize = 5);
	// randomize board
	void CreateNewAnswer();
	void CreateBoard();
};

class Game {
public:
	Game();
	~Game();

	void NewGame(const vec2D& screenCenter, int numTiles = 5, int tileSize = 5);
	void Update(olc::PixelGameEngine* engine, float& fElapsedTime);
	void Draw(olc::PixelGameEngine* engine);
	void InitUI();

	GameGrid gameGrid;
	GameState gameState;
private:
	bool _mouseDown;
	TileState _mouseActiveState;
	vec2D _winStrPos;
	float _stopwatch;
	vec2D _stopwatchPos;
	vec2D _hintStrScale = { 1.0f, 1.0f };

	olc::QuickGUI::Manager _guiManager;
	olc::QuickGUI::Button* _clearBtn = nullptr;
	olc::QuickGUI::Button* _newGameBtn = nullptr;
	olc::QuickGUI::Label* _showCorrectLabel = nullptr;
	olc::QuickGUI::CheckBox* _showCorrectCB = nullptr;
	std::vector<std::string> _boardSizesStr = { "5x5", "10x10", "15x15", "20x20" };
	olc::QuickGUI::ListBox* _boardSizeDropdown = nullptr;
	
	bool _matchesAnswer();
	inline void _clearGameGrid();
	
};

#endif