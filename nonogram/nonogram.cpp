// nonogram.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

#include "util.h"
#include "game.h"

class App : public olc::PixelGameEngine {
public:
    App() { sAppName = "nonogram"; }

public:
    bool OnUserCreate() override {
        _game = new Game();
        _game->gameGrid.pos = { 100, 100 };
        _game->NewGame(10, 10);
        // _game->NewGame(5, 10);
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::WHITE);
        _game->Update(this);
        _game->Draw(this);
        return true;
    }

    bool OnUserDestroy() override {
        delete _game;
        return true;
    }

private:
    olc::QuickGUI::Manager _guiManager;
    Game* _game;
};

int main()
{
    App game;

    if (game.Construct(800, 600, 1, 1)) {
        game.Start();
    }
    
    return 0;
}
