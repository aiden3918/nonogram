#include "game.h"

GameTile::GameTile() {}
GameTile::GameTile(const vec2D& gridPos, const int& tileSize, const int& row, const int& column)
{
	tSize = tileSize;
	r = row;
	c = column;
	pos = gridPos + (vec2D(c, r) * tSize) + vec2D(c / 5, r / 5);
	hitbox = { pos, pos + vec2D(tSize, tSize) };
	active = false;
	state = TileState::EMPTY;
}
GameTile::~GameTile() {}

void GameGrid::Clear(const int& numTiles, const int& tileSize) {
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

void GameGrid::CreateNewAnswer() {
	srand(time(NULL));
	std::vector<bool> emptyRow;
	emptyRow.resize(answer[0].size(), 0);

	for (int r = 0; r < answer.size(); r++) {
		int rowHintIndex = 0;
		_horizontalHints[r].first.push_back(0);

		// randomize row
		for (int c = 0; c < answer[r].size(); c++) answer[r][c] = rand() % 2;

		// if row is empty, add a one in there somewhere
		if (answer[r] == emptyRow) answer[r][rand() % emptyRow.size()] == 1;

		// side hints (horizontal)
		for (int c = 0; c < answer[r].size(); c++) {
			switch (answer[r][c]) {
			case 0: {
				if (_horizontalHints[r].first[rowHintIndex] != 0) {
					rowHintIndex++;
					_horizontalHints[r].first.push_back(0);
				}
				break;
			}
			case 1: _horizontalHints[r].first[rowHintIndex]++; break;
			}
		}

		// might be a 0 at the end
		if (_horizontalHints[r].first.back() == 0) _horizontalHints[r].first.pop_back();

		// turn vector int into string
		std::stringstream ss;
		std::copy(_horizontalHints[r].first.begin(), _horizontalHints[r].first.end(),
			std::ostream_iterator<int>(ss, " "));
		_horizontalHints[r].second = ss.str();
	}

	// vertical hints
	for (int c = 0; c < answer[0].size(); c++) {
		int columnHintIndex = 0;
		_verticalHints[c].first.push_back(0);

		for (int r = 0; r < answer.size(); r++) {
			switch (answer[r][c]) {
			case 0: {
				if (_verticalHints[c].first[columnHintIndex] != 0) {
					columnHintIndex++;
					_verticalHints[c].first.push_back(0);
				}
				break;
			}
			case 1: _verticalHints[c].first[columnHintIndex]++; break;
			}
		}

		if (_verticalHints[c].first.back() == 0) _verticalHints[c].first.pop_back();

		// turn vector int into string
		std::stringstream ss;
		std::copy(_verticalHints[c].first.begin(), _verticalHints[c].first.end(),
			std::ostream_iterator<int>(ss, "\n\n"));
		_verticalHints[c].second = ss.str();
	}

	// cout2DVec<int>(_horizontalHints);
	// cout2DVec<int>(_verticalHints);
	cout2DVec<bool>(answer);
}

void GameGrid::CreateBoard() {
	tiles.clear();

	for (int r = 0; r < answer.size(); r++) {
		for (int c = 0; c < answer[r].size(); c++) {
			GameTile tile = GameTile(pos, tSize, r, c);
			tiles.push_back(tile);
		}
	}
}

Game::Game() {}
Game::~Game() {}

void Game::NewGame(int numTiles, int tileSize) {
	gameGrid.Clear(numTiles, tileSize);
	gameGrid.CreateNewAnswer();
	gameGrid.CreateBoard();

	std::cout << gameGrid.tiles.size() << std::endl;
}

void Game::Update(olc::PixelGameEngine* engine) {
	vec2D mousePos = { engine->GetMouseX(), engine->GetMouseY() };

	bool leftHeld = engine->GetMouse(olc::Mouse::LEFT).bHeld;
	bool rightHeld = engine->GetMouse(olc::Mouse::RIGHT).bHeld;
	if (!leftHeld && !rightHeld) {
		_mouseDown = false;
		return;
	}

	for (GameTile& t : gameGrid.tiles) {
		if (!checkPtCollision(mousePos, t.hitbox)) continue;

		if (leftHeld) {
			if (_mouseDown) {
				t.active = static_cast<int>(_mouseActiveState);
				(t.active) ? t.state = TileState::FILLED : t.state = TileState::EMPTY;
			}
			else {
				_mouseActiveState = static_cast<TileState>(!t.active);
				_mouseDown = true;
			}
		}
		else {
			// WIP
			t.active = false;
			if (_mouseDown) {
				t.active = static_cast<int>(_mouseActiveState);
				(t.active) ? t.state = TileState::FILLED : t.state = TileState::EMPTY;
			}
			else {
				_mouseActiveState = static_cast<TileState>(!t.active);
				_mouseDown = true;
			}
			(t.state == TileState::FLAGGED) ? t.state = TileState::EMPTY : 
				t.state = TileState::FLAGGED;
			_mouseDown = true;
		}

	}

	if (_matchesAnswer()) std::cout << "win" << std::endl;
}

void Game::Draw(olc::PixelGameEngine* engine) {
	vec2D mousePos = { engine->GetMouseX(), engine->GetMouseY() };
	engine->DrawStringDecal({ 5.0f, 5.0f }, "Mouse Pos: " + std::to_string(mousePos.x) + ", " +
		std::to_string(mousePos.y), olc::BLACK);

	// draw game tiles
	for (GameTile& t : gameGrid.tiles) {
		engine->DrawRectDecal({ t.pos.x, t.pos.y }, { (float)t.tSize, (float)t.tSize }, 
			olc::BLACK);
		switch (t.state) {
		case TileState::EMPTY: break;
		case TileState::FILLED: engine->FillRectDecal({ t.pos.x, t.pos.y },
			{ (float)t.tSize, (float)t.tSize }, olc::BLACK); break;
		case TileState::FLAGGED: {
			engine->DrawLineDecal({ t.hitbox.min.x + 2, t.hitbox.min.y + 2 },
				{ t.hitbox.max.x - 2, t.hitbox.max.y - 2 }, olc::RED);
			engine->DrawLineDecal({ t.hitbox.max.x - 2, t.hitbox.min.y + 2 },
				{ t.hitbox.min.x + 2, t.hitbox.max.y - 2 }, olc::RED);
			break;
		}
		}
			
	}

	// draw horizontal hints
	int stringSpacing = (gameGrid.tiles[0].tSize);
	for (int h = 0; h < gameGrid._horizontalHints.size(); h++) {
		engine->DrawStringDecal({gameGrid.pos.x - 50, gameGrid.pos.y + h * stringSpacing },
			gameGrid._horizontalHints[h].second, olc::BLACK);
	}

	// draw vertical hints
	for (int v = 0; v < gameGrid._verticalHints.size(); v++) {
		engine->DrawStringDecal({ gameGrid.pos.x + v * stringSpacing, gameGrid.pos.y - 50},
			gameGrid._verticalHints[v].second, olc::BLACK);
	}
}

// DEBUG: vector subscript out of range
inline bool Game::_matchesAnswer() {
	for (GameTile& t : gameGrid.tiles) {
		if (gameGrid.answer[t.r][t.c] != t.active) return false;
	}
	return true;
}