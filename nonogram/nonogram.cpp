// nonogram.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

#include "util.h"
#include "game.h"
#include <random>

class App : public olc::PixelGameEngine {
public:
    App() { sAppName = "nonogram"; }

public:
    bool OnUserCreate() override {
        vec2D screenCenter = GetScreenSize() / 2.0f;

        srand(time(NULL));

        _game = new Game();
        _game->NewGame(screenCenter, 5, 15);
        _game->InitUI();
        // _game->NewGame(5, 10);
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::WHITE);
        _game->Update(this, fElapsedTime);
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
