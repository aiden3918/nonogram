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

void GameGrid::Clear(const vec2D& screenCenter, const int& numTiles, const int& tileSize) {
	horiDisp = 0;
	vertDisp = 0;
	tiles.clear();

	answer.clear();
	horizontalHints.clear();
	verticalHints.clear();

	answer.resize(numTiles);
	tSize = tileSize;
	for (std::vector<bool>& v : answer) v.resize(numTiles, false);

	horizontalHints.resize(numTiles);
	verticalHints.resize(numTiles);

	cout2DVec<bool>(answer);
	gridSize = vec2D(int((answer[0].size() * tSize) + (answer[0].size() / 5)),
		int((answer.size() * tSize) + (answer.size() / 5)));
	center = screenCenter;
	pos = screenCenter - (gridSize / 2.0f);
	pos.y *= 1.4f;
	gameTilesDimensions = { pos, pos + gridSize };
}

void GameGrid::CreateNewAnswer() {
	srand(time(NULL));
	std::vector<bool> emptyRow;
	emptyRow.resize(answer[0].size(), 0);

	for (int r = 0; r < answer.size(); r++) {
		int rowHintIndex = 0;
		horizontalHints[r].vec.push_back(0);

		// randomize row
		for (int c = 0; c < answer[r].size(); c++) answer[r][c] = rand() % 2;

		// if row is empty, add a one in there somewhere
		if (answer[r] == emptyRow) answer[r][rand() % emptyRow.size()] == 1;

		// side hints (horizontal)
		for (int c = 0; c < answer[r].size(); c++) {
			switch (answer[r][c]) {
			case 0: {
				if (horizontalHints[r].vec[rowHintIndex] != 0) {
					rowHintIndex++;
					horizontalHints[r].vec.push_back(0);
				}
				break;
			}
			case 1: horizontalHints[r].vec[rowHintIndex]++; break;
			}
		}

		// might be a 0 at the end
		if (horizontalHints[r].vec.back() == 0) horizontalHints[r].vec.pop_back();

		// turn vector int into string
		std::stringstream ss;
		std::copy(horizontalHints[r].vec.begin(), horizontalHints[r].vec.end(),
			std::ostream_iterator<int>(ss, " "));
		horizontalHints[r].str = ss.str();
		// _horizontalHints[r].second.substr(0, _horizontalHints[r].second.length() - 1);
		if (horizontalHints[r].str.length() > horiDisp)
			horiDisp = horizontalHints[r].str.length();
	}

	// vertical hints
	for (int c = 0; c < answer[0].size(); c++) {
		int columnHintIndex = 0;
		verticalHints[c].vec.push_back(0);

		for (int r = 0; r < answer.size(); r++) {
			switch (answer[r][c]) {
			case 0: {
				if (verticalHints[c].vec[columnHintIndex] != 0) {
					columnHintIndex++;
					verticalHints[c].vec.push_back(0);
				}
				break;
			}
			case 1: verticalHints[c].vec[columnHintIndex]++; break;
			}
		}

		if (verticalHints[c].vec.back() == 0) verticalHints[c].vec.pop_back();

		// turn vector int into string
		std::stringstream ss;
		std::copy(verticalHints[c].vec.begin(), verticalHints[c].vec.end(),
			std::ostream_iterator<int>(ss, "\n\n"));
		verticalHints[c].str = ss.str();
		if (verticalHints[c].str.length() > vertDisp)
			vertDisp = verticalHints[c].str.length();
	}

	horiDisp *= tSize / 1.5;
	vertDisp *= tSize / 2;

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

void Game::NewGame(const vec2D& screenCenter, int numTiles, int tileSize) {
	gameGrid.Clear(screenCenter, numTiles, tileSize);
	gameGrid.CreateNewAnswer();
	gameGrid.CreateBoard();

	std::cout << gameGrid.tiles.size() << std::endl;
}

void Game::Update(olc::PixelGameEngine* engine) {
	_guiManager.Update(engine);
	vec2D mousePos = engine->GetMousePos();

	if (_clearBtn->bPressed) _clearGameGrid();
	if (_newGameBtn->bPressed) {
		_clearGameGrid();
		NewGame(engine->GetScreenSize() / 2, 
			gameGrid.answer[0].size(), gameGrid.tSize);
	}

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
		else if (rightHeld) {
			t.active = false;
			if (_mouseDown) t.state = _mouseActiveState;
			else {
				(t.state == TileState::FLAGGED) ?
					_mouseActiveState = TileState::EMPTY : 
					_mouseActiveState = TileState::FLAGGED;
				_mouseDown = true;
			}
		}

	}

	if (_matchesAnswer()) std::cout << "win" << std::endl;
}

void Game::Draw(olc::PixelGameEngine* engine) {
	vec2D mousePos = vec2D(engine->GetMousePos());
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

	for (int h = 0; h < gameGrid.horizontalHints.size(); h++) {
		if (gameGrid.horizontalHints[h].correct && _showCorrectCB->bChecked) {
			engine->DrawStringDecal({ gameGrid.pos.x - gameGrid.horiDisp + 3,
				gameGrid.pos.y + h * stringSpacing + (h / 5) + 3 }, 
				gameGrid.horizontalHints[h].str, olc::GREEN);
		} 
		else {
			engine->DrawStringDecal({ gameGrid.pos.x - gameGrid.horiDisp + 3,
				gameGrid.pos.y + h * stringSpacing + (h / 5) + 3 },
				gameGrid.horizontalHints[h].str, olc::BLACK);
		}

		engine->DrawRectDecal({ gameGrid.pos.x - gameGrid.horiDisp,
			gameGrid.pos.y + h * stringSpacing + (h / 5)}, {(float)gameGrid.horiDisp, (float)gameGrid.tSize},
			olc::BLACK);
	}

	// draw vertical hints
	for (int v = 0; v < gameGrid.verticalHints.size(); v++) {
		if (gameGrid.verticalHints[v].correct && _showCorrectCB->bChecked) {
			engine->DrawStringDecal({ gameGrid.pos.x + v * stringSpacing + 3,
				gameGrid.pos.y - gameGrid.vertDisp + 3 }, gameGrid.verticalHints[v].str,
				olc::GREEN);
		}
		else {
			engine->DrawStringDecal({ gameGrid.pos.x + v * stringSpacing + 3,
				gameGrid.pos.y - gameGrid.vertDisp + 3 }, gameGrid.verticalHints[v].str,
				olc::BLACK);
		}
		engine->DrawRectDecal({ gameGrid.pos.x + v * stringSpacing + (v / 5),
			gameGrid.pos.y - gameGrid.vertDisp }, { (float)gameGrid.tSize, (float)gameGrid.vertDisp},
			olc::BLACK);
	}

	engine->FillRectDecal({ 0.0f, 0.0f }, { (float)engine->GetScreenSize().x, {50.0f} }, 
		olc::BLACK);
	engine->DrawStringDecal({ 10.0f, 10.0f }, "Nonograms", olc::WHITE, { 3.0f, 3.0f });
	_guiManager.Draw(engine);
}

void Game::InitUI() {
	_clearBtn = new olc::QuickGUI::Button(_guiManager, "Clear",
		{ gameGrid.gameTilesDimensions.min.x - gameGrid.horiDisp, gameGrid.gameTilesDimensions.max.y }, 
		{ 50.0f, 30.0f });
	_newGameBtn = new olc::QuickGUI::Button(_guiManager, "New Game",
		{ gameGrid.gameTilesDimensions.min.x + 60.0f - gameGrid.horiDisp, gameGrid.gameTilesDimensions.max.y },
		{ 80.0f, 30.0f });
	_showCorrectLabel = new olc::QuickGUI::Label(_guiManager, "Show Correct Rows/Cols",
		{ gameGrid.gameTilesDimensions.min.x + 150.0f - gameGrid.horiDisp,
		gameGrid.gameTilesDimensions.max.y }, { 160.0f, 30.0f });
	_showCorrectLabel->bHasBackground = true;
	_showCorrectLabel->bHasBorder = true;
	_showCorrectCB = new olc::QuickGUI::CheckBox(_guiManager, "",
		false, { gameGrid.gameTilesDimensions.min.x + 310.0f - gameGrid.horiDisp,
		gameGrid.gameTilesDimensions.max.y + 5.0f}, { 20.0f, 20.0f });
}

inline bool Game::_matchesAnswer() {
	bool correct = true;

	for (HintData& h : gameGrid.horizontalHints) h.correct = true;
	for (HintData& v : gameGrid.horizontalHints) v.correct = true;

	for (GameTile& t : gameGrid.tiles) {

		if (gameGrid.answer[t.r][t.c] != t.active) {
			correct = false;
			gameGrid.horizontalHints[t.r].correct = false;
			gameGrid.verticalHints[t.c].correct = false;
		}
	}
	return correct;
}

inline void Game::_clearGameGrid() {
	for (GameTile& t : gameGrid.tiles) {
		t.active = false;
		t.state = TileState::EMPTY;
	}
} 