#pragma once

#ifndef game_h
#define game_h

#include "util.h"
#include <random>
#include "olcPixelGameEngine.h"
#include "olcPGEX_QuickGUI.h"

struct GameTile {
	vec2D pos;
	AABB hitbox;
	int tSize;
	bool active;
	int r; // "y" value
	int c; // "x" value

	GameTile() {}
	GameTile(const vec2D& gridPos, const int& tileSize, const int& row, const int& column,
		bool isActive = false)
	{
		tSize = tileSize;
		r = row;
		c = column;
		active = isActive;
		pos = gridPos + (vec2D(c, r) * tSize) + vec2D(c / 5, r / 5);
		hitbox = { pos, pos + vec2D(tSize, tSize) };
	}
	~GameTile() {}
};

struct GameGrid {
	// 5x5, 10x10, 15x15, 20x20
	std::vector<GameTile> tiles;
	std::vector<std::vector<int>> _horizontalHints; // sides that give hints (element 0 is horizntal, element 1 is vertical)
	std::vector<std::vector<int>> _verticalHints; // sides that give hints (element 0 is horizntal, element 1 is vertical)
	std::vector<std::vector<bool>> answer;

	vec2D pos; // top-left of where clickable tiles starts
	int tSize;

	// init with tiles
	void Clear(const int& numTiles, const int& tileSize = 5) {
		tiles.clear();

		answer.clear();
		_horizontalHints.clear();
		_verticalHints.clear();

		answer.resize(numTiles);
		tSize = tileSize;
		for (std::vector<bool>& v : answer) v.resize(numTiles, false);

		_horizontalHints.resize(numTiles);
		_verticalHints.resize(numTiles);

		cout2DVec<bool>(answer);
	}

	// randomize board
	void CreateNewAnswer() {
		srand(time(NULL));
		std::vector<bool> emptyRow;
		emptyRow.resize(answer[0].size(), 0);

		for (int r = 0; r < answer.size(); r++) {
			int rowHintIndex = 0;
			_horizontalHints[r].push_back(0);

			// randomize row
			for (int c = 0; c < answer[r].size(); c++) answer[r][c] = rand() % 2;
			
			// if row is empty, add a one in there somewhere
			if (answer[r] == emptyRow) answer[r][rand() % emptyRow.size()] == 1;

			// side hints (horizontal)
			for (int c = 0; c < answer[r].size(); c++) {
				switch (answer[r][c]) {
				case 0: {
					if (_horizontalHints[r][rowHintIndex] != 0) {
						rowHintIndex++;
						_horizontalHints[r].push_back(0);
					}
					break;
				}
				case 1: _horizontalHints[r][rowHintIndex]++; break;
				}
			}

			// might be a 0 at the end
			if (_horizontalHints[r].back() == 0) _horizontalHints[r].pop_back();
		}
	
		// DEBUG: vector subscript probably out of range
		// vertical hints
		for (int c = 0; c < answer[0].size(); c++) {
			int columnHintIndex = 0;
			_verticalHints[c].push_back(0);
			
			for (int r = 0; r < answer.size(); r++) {
				switch (answer[r][c]) {
				case 0: {
					if (_verticalHints[c][columnHintIndex] != 0) {
						columnHintIndex++;
						_verticalHints[c].push_back(0);
					}
					break;
				}
				case 1: _verticalHints[c][columnHintIndex]++; break;
				}
			}

			if (_verticalHints[c].back() == 0) _verticalHints[c].pop_back();
		}
			
		cout2DVec<int>(_horizontalHints);
		cout2DVec<int>(_verticalHints);
		cout2DVec<bool>(answer);
	}

	void CreateBoard() {
		tiles.clear();
		tiles.resize(answer.size());

		for (int r = 0; r < answer.size(); r++) {
			for (int c = 0; c < answer[r].size(); c++) {
				GameTile tile = GameTile(pos, tSize, r, c);
				tiles.push_back(tile);
			}
		}
	}
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
	bool _mouseActiveState;
	bool _matchAnswer();
	
};

#endif