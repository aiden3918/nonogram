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
	if (numTiles > 10) center.y *= 1.3f;
	pos = center - (gridSize / 2.0f);
	gameTilesDimensions = { pos, pos + gridSize };

}

void GameGrid::CreateNewAnswer() {
	srand(time(NULL));
	std::vector<bool> emptyRow;
	int cols = answer[0].size();
	int rows = answer.size();
	emptyRow.resize(cols, 0);

	for (int r = 0; r < rows; r++) {
		int rowHintIndex = 0;
		horizontalHints[r].vec.push_back(0);

		// randomize row
		for (int c = 0; c < cols; c++) answer[r][c] = rand() % 2;

		// if row is empty, add a one in there somewhere
		if (answer[r] == emptyRow) answer[r][rand() % emptyRow.size()] == 1;

		// side hints (horizontal)
		for (int c = 0; c < cols; c++) {
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

	// check for empty columns
	// would be easier to just insert a random 1 somewhere no matter what, but init
	// is called only once per game so why not use some extra cpu power :)
	for (int c = 0; c < cols; c++) {
		bool emptyCol = true;
		for (int r = 0; r < rows; r++) {
			if (answer[r][c] != 0) {
				emptyCol = false;
				break;
			}
		}
		if (emptyCol) answer[rand() % rows][c] = 1;
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

	horiDisp *= tSize / 1.5f;
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

	gameState = GameState::PLAYING;

	gameGrid.Clear(screenCenter, numTiles, tileSize);
	gameGrid.CreateNewAnswer();
	gameGrid.CreateBoard();

	_winStrPos = vec2D(screenCenter.x - 20.0f, gameGrid.gameTilesDimensions.max.y + 40.0f);

	_stopwatch = 0.0f;
	_stopwatchPos = vec2D(screenCenter.x, gameGrid.pos.y - gameGrid.vertDisp - 20.0f);

	_clearBtn->vPos = { gameGrid.gameTilesDimensions.min.x - gameGrid.horiDisp, 
		gameGrid.gameTilesDimensions.max.y };
	_newGameBtn->vPos = { gameGrid.gameTilesDimensions.min.x + 60.0f - gameGrid.horiDisp,
		gameGrid.gameTilesDimensions.max.y };
	_showCorrectLabel->vPos = { gameGrid.gameTilesDimensions.min.x + 150.0f - gameGrid.horiDisp,
		gameGrid.gameTilesDimensions.max.y };
	_showCorrectCB->vPos = { gameGrid.gameTilesDimensions.min.x + 310.0f - gameGrid.horiDisp, 
		gameGrid.gameTilesDimensions.max.y + 5.0f };

	std::cout << gameGrid.tiles.size() << std::endl;
}

void Game::Update(olc::PixelGameEngine* engine, float& fElapsedTime) {
	_guiManager.Update(engine);
	vec2D mousePos = engine->GetMousePos();

	if (gameState == GameState::PLAYING) _stopwatch += fElapsedTime;

	if (_clearBtn->bPressed) _clearGameGrid();
	if (_newGameBtn->bPressed) {
		_clearGameGrid();
		NewGame(engine->GetScreenSize() / 2, 
			gameGrid.answer[0].size(), gameGrid.tSize);
		return;
	}

	if (_boardSizeDropdown->nPreviouslySelectedItem != _boardSizeDropdown->nSelectedItem) {
		int numTiles = (_boardSizeDropdown->nSelectedItem + 1) * 5;
		int tileSize = 15;
		//_hintStrScale = vec2D(1.0f, 1.0f);
		//if (_boardSizeDropdown->nSelectedItem > 1) {
		//	tileSize = 10;
		//	_hintStrScale = vec2D(0.8f, 0.8f);
		//};
		NewGame(engine->GetScreenSize() / 2, numTiles, tileSize);
		return;
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

	if (_matchesAnswer()) gameState = GameState::WIN;
}

void Game::Draw(olc::PixelGameEngine* engine) {
	vec2D mousePos = vec2D(engine->GetMousePos());
	engine->DrawStringDecal({ 5.0f, 550.0f }, "Mouse Pos: " + std::to_string(mousePos.x) + ", " +
		std::to_string(mousePos.y), olc::BLACK);
	engine->FillRectDecal({ 0.0f, 0.0f }, { (float)engine->GetScreenSize().x, {50.0f} },
		olc::BLACK);
	engine->DrawStringDecal({ 10.0f, 10.0f }, "Nonograms", olc::WHITE, { 3.0f, 3.0f });

	// draw timer
	std::string hrStr = std::to_string(int(_stopwatch / 3600.0f));
	std::string minStr = std::to_string(int(_stopwatch / 60.0f) % 60);
	if (minStr.length() < 2) minStr.insert(0, "0");
	std::string secStr = std::to_string(int(_stopwatch) % 60);
	if (secStr.length() < 2) secStr.insert(0, "0");
	engine->DrawStringDecal({ _stopwatchPos.x - 25.0f, _stopwatchPos.y }, 
		hrStr + ":" + minStr + ":" + secStr, olc::BLACK);

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

	olc::vf2d strScale = { _hintStrScale.x, _hintStrScale.y };
	for (int h = 0; h < gameGrid.horizontalHints.size(); h++) {
		if (gameGrid.horizontalHints[h].correct && _showCorrectCB->bChecked) {
			engine->DrawStringDecal({ gameGrid.pos.x - gameGrid.horiDisp + 3,
				gameGrid.pos.y + h * stringSpacing + (h / 5) + 3 }, 
				gameGrid.horizontalHints[h].str, olc::GREEN, strScale);
		} 
		else {
			engine->DrawStringDecal({ gameGrid.pos.x - gameGrid.horiDisp + 3,
				gameGrid.pos.y + h * stringSpacing + (h / 5) + 3 },
				gameGrid.horizontalHints[h].str, olc::BLACK, strScale);
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
				olc::GREEN, strScale);
		}
		else {
			engine->DrawStringDecal({ gameGrid.pos.x + v * stringSpacing + 3,
				gameGrid.pos.y - gameGrid.vertDisp + 3 }, gameGrid.verticalHints[v].str,
				olc::BLACK, strScale);
		}
		engine->DrawRectDecal({ gameGrid.pos.x + v * stringSpacing + (v / 5),
			gameGrid.pos.y - gameGrid.vertDisp }, { (float)gameGrid.tSize, (float)gameGrid.vertDisp},
			olc::BLACK);
	}

	if (gameState == GameState::WIN) engine->DrawStringDecal({ _winStrPos.x, _winStrPos.y }, 
		"Win!", olc::GREEN, {2.0f, 2.0f});

	_guiManager.DrawDecal(engine);

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
	_boardSizeDropdown = new olc::QuickGUI::ListBox(_guiManager, _boardSizesStr,
		{ 550.0f, 10.0f }, { 70.0f, 30.0f });
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