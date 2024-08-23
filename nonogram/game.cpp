#include "game.h"

Game::Game() {}
Game::~Game() {}

void Game::NewGame() {
	_gameGrid.Clear(5, 5);
	_gameGrid.NewBoard();

	//_gameGrid.Init(20);
	//_gameGrid.NewBoard();
}