/**
 * File        : Gui.cc
 * Description : The main GUI interface for running the parallel path-finding algorithms on
 */

#include <iostream>
#include <memory>
#include <random>

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "common/World.h"
#include "gui/Error.h"
#include "gui/Window.h"
#include "gui/TextInput.h"
#include "gui/WorldViewport.h"

using namespace pathFind;

bool sdl_init ();

const size_t SCREEN_WIDTH = 1280;
const size_t SCREEN_HEIGHT = 960;

int main (int, char**)
{
    bool quit = !sdl_init ();
    Log::logInfo("SDL started.");

    gui::Window window("Parallel Path Finding", SCREEN_WIDTH, SCREEN_HEIGHT);

    std::shared_ptr<gui::Viewport> mainViewport = std::make_shared<gui::Viewport> (
            SDL_Rect { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },
            SDL_Color {0xAA, 0x37, 0x4A, 0xFF});
    std::shared_ptr<gui::WorldViewport> worldViewport = std::make_shared<gui::WorldViewport> (
            SDL_Rect {0, 0, SCREEN_WIDTH, (SCREEN_HEIGHT * 7) / 8 },
            SDL_Color {0xBB, 0xCC, 0xCC, 0xFF});
    std::shared_ptr<gui::Viewport> toolbarViewport = std::make_shared<gui::Viewport> (
            SDL_Rect {0, (SCREEN_HEIGHT * 7) / 8, SCREEN_WIDTH, SCREEN_HEIGHT / 8},
            SDL_Color {0x88, 0xAA, 0x88, 0xFF});

    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    std::shared_ptr<gui::TextInput> genWorldInput_TI;
    auto genWorld_B = std::make_shared<gui::Button> (mainViewport->getX (), mainViewport->getY (),
            "Generate World", SDL_Rect
            { (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 25, 240, 50 }, 16,
            [&]()
            {
                genWorldInput_TI->enable();
            });

    std::shared_ptr<gui::Button> regenerate_B;

    std::string worldCommand;
    std::function<void(std::string)> genWorldFunct = [&](std::string s)
            {
                worldCommand = s;
                std::string command = "./worldGen " + worldCommand;
                system (command.c_str ());

                std::string worldName = worldCommand;
                int pos = worldName.find_first_of(" ");
                worldName.erase(pos, std::string::npos);

                worldViewport->setWorld(worldName);
                worldViewport->loadWorld();

                worldViewport->enable();
                toolbarViewport->enable();
                regenerate_B->enable ();
                mainViewport->disable();
            };

    genWorldInput_TI = std::make_shared<gui::TextInput> (
            (SCREEN_WIDTH / 2), SCREEN_HEIGHT - 75, 16, genWorldFunct);

    std::shared_ptr<gui::TextInput> viewWorldInput_TI;
    auto viewWorld_B = std::make_shared<gui::Button> (
            worldViewport->getX (), worldViewport->getY (),
            "View World", SDL_Rect
            {(SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 100, 240, 50}, 16,
            [&]()
            {
                viewWorldInput_TI->enable ();
            });

    std::shared_ptr<gui::Button> viewResult_B;

    viewWorldInput_TI = std::make_shared<gui::TextInput> (
            (SCREEN_WIDTH / 2), SCREEN_HEIGHT - 75, 16,
            [&](std::string s)
            {
                worldViewport->setWorld(s);
                worldViewport->loadWorld();

                worldViewport->enable();
                toolbarViewport->enable();
                viewResult_B->enable();
                mainViewport->disable();
            });

    auto backToMenu_B = std::make_shared<gui::Button> (
            toolbarViewport->getX (), toolbarViewport->getY (),
            "Back To Menu", SDL_Rect
            {20, toolbarViewport->getHeight() / 2 - 25, 280, 50}, 16,
            [&]()
            {
                mainViewport->enable ();
                worldViewport->setResultsEnabled(false);
                worldViewport->disable ();
                viewResult_B->disable ();
                regenerate_B->disable ();
                toolbarViewport->disable ();
            });

    regenerate_B = std::make_shared<gui::Button> (
            toolbarViewport->getX (), toolbarViewport->getY (),
            "Regenerate World", SDL_Rect
            {320, toolbarViewport->getHeight() / 2 - 25, 280, 50}, 16,
            [&]()
            {
                std::string command = "./worldGen " + worldCommand;
                system (command.c_str ());

                worldViewport->loadWorld();
            });

    std::shared_ptr<gui::TextInput> viewResult_TI;

    viewResult_B = std::make_shared<gui::Button> (
            toolbarViewport->getX (), toolbarViewport->getY (),
            "View Results", SDL_Rect
            {320, toolbarViewport->getHeight () / 2 - 25, 280, 50}, 16,
            [&]()
            {
                viewResult_TI->enable();
                viewResult_B->disable();
            });

    viewResult_TI = std::make_shared<gui::TextInput> (
            460, toolbarViewport->getHeight () / 2 - 25, 16,
            [&](std::string s)
            {
                std::stringstream input (s);
                std::string alg;
                uint sx, sy, ex, ey;
                input >> alg >> sx >> sy >> ex >> ey;
                worldViewport->runAndLoadPathFinding(alg, sx, sy, ex, ey);
                worldViewport->setResultsEnabled(true);
                viewResult_B->enable ();
            });

    mainViewport->addButton (genWorld_B);
    mainViewport->addButton (genWorldInput_TI);
    genWorld_B->enable ();
    mainViewport->addButton (viewWorld_B);
    mainViewport->addButton (viewWorldInput_TI);
    viewWorld_B->enable ();

    mainViewport->enable ();

    toolbarViewport->addButton (regenerate_B);
    toolbarViewport->addButton (backToMenu_B);
    toolbarViewport->addButton (viewResult_B);
    toolbarViewport->addButton (viewResult_TI);
    backToMenu_B->enable ();

    window.addViewport (mainViewport);
    window.addViewport (worldViewport);
    window.addViewport(toolbarViewport);

    SDL_Event e;

    window.spawnWindow ();
    window.render ();

    // application loop
    while (!quit)
    {
        while (SDL_PollEvent (&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else
            {
                window.handleEvent (e);
            }
        }
        window.render ();
    }
    Log::logInfo ("Exiting program.");

    SDL_Quit ();

    return EXIT_SUCCESS;
}

bool sdl_init ()
{
    // SDL initialization
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
        Log::logError (
                "Failed to initialized SDL. SDL_Error: "
                        + std::string (SDL_GetError ()) + "\n");
        return false;
    }

    if (TTF_Init () == -1)
    {
        Log::logError (
                "SDL_ttf could not initialize. SDL_ttf Error: "
                        + std::string (TTF_GetError ()) + "\n");
        return false;
    }

    return true;
}
