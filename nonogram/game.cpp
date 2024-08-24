#include "game.h"

Game::Game() {}
Game::~Game() {}

void Game::NewGame(int numTiles, int tileSize) {
	gameGrid.Clear(numTiles, tileSize);
	gameGrid.CreateNewAnswer();
	gameGrid.CreateBoard();
}

void Game::Update(olc::PixelGameEngine* engine) {
	vec2D mousePos = { engine->GetMouseX(), engine->GetMouseY() };

	if (!engine->GetMouse(olc::Mouse::LEFT).bHeld) {
		_mouseDown = false;
		return;
	}

	for (GameTile& t : gameGrid.tiles) {
		if (checkPtCollision(mousePos, t.hitbox)) {
			if (_mouseDown) {
				t.active = _mouseActiveState;
			}
			else {
				_mouseActiveState = !t.active;
				_mouseDown = true;
			}
		}
	}
}

void Game::Draw(olc::PixelGameEngine* engine) {
	vec2D mousePos = { engine->GetMouseX(), engine->GetMouseY() };
	engine->DrawStringDecal({ 5.0f, 5.0f }, "Mouse Pos: " + std::to_string(mousePos.x) + ", " +
		std::to_string(mousePos.y), olc::BLACK);

	for (GameTile& t : gameGrid.tiles) {
		(t.active) ? engine->FillRectDecal({ t.pos.x, t.pos.y }, { (float)t.tSize, (float)t.tSize }, olc::BLACK) : 
			engine->DrawRectDecal({ t.pos.x, t.pos.y }, { (float)t.tSize, (float)t.tSize }, olc::BLACK);
	}
}