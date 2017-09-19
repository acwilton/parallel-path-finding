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

using namespace parPath;

bool sdl_init ();

const size_t SCREEN_WIDTH = 1280;
const size_t SCREEN_HEIGHT = 960;

int main (int args, char* argv[])
{
    bool quit = !sdl_init ();
    Log::logInfo("SDL started.");

    Window window("Parallel Path Finding", SCREEN_WIDTH, SCREEN_HEIGHT);

    std::shared_ptr<Viewport> mainViewport = std::make_shared<Viewport> (
            SDL_Rect { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },
            SDL_Color {0xAA, 0x37, 0x4A});
    std::shared_ptr<WorldViewport> worldViewport = std::make_shared<WorldViewport> (
            SDL_Rect {0, 0, SCREEN_WIDTH, (SCREEN_HEIGHT * 7) / 8 },
            SDL_Color {0xBB, 0xCC, 0xCC});
    std::shared_ptr<Viewport> toolbarViewport = std::make_shared<Viewport> (
            SDL_Rect {0, (SCREEN_HEIGHT * 7) / 8, SCREEN_WIDTH, SCREEN_HEIGHT / 8},
            SDL_Color {0x88, 0xAA, 0x88});

    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    std::shared_ptr<TextInput> genWorldInput_TI;
    auto genWorld_B = std::make_shared<Button> (mainViewport->getX (), mainViewport->getY (),
            "Generate World", SDL_Rect
            { (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 25, 240, 50 }, 16,
            [&]()
            {
                genWorldInput_TI->enable();
            });

    std::string worldCommand;
    std::function<void(std::string)> genWorldFunct = [&](std::string s)
            {
                worldCommand = s;
                std::string command = "./worldGen " + worldCommand;
                system (command.c_str ());

                std::string worldName = worldCommand;
                int pos = worldName.find_first_of(" ");
                worldName.erase(pos, std::string::npos);

                worldViewport->setFile(worldName);
                worldViewport->loadFile();

                worldViewport->enable();
                toolbarViewport->enable();
                mainViewport->disable();
                //sizeInput_TI->enable();
            };

    genWorldInput_TI = std::make_shared<TextInput> (
            (SCREEN_WIDTH / 2), SCREEN_HEIGHT - 75, 16, genWorldFunct);

    std::shared_ptr<TextInput> viewWorldInput_TI;
    auto viewWorld_B = std::make_shared<Button> (
            worldViewport->getX (), worldViewport->getY (),
            "View World", SDL_Rect
            {(SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 100, 240, 50}, 16,
            [&]()
            {
                viewWorldInput_TI->enable ();
            });

    viewWorldInput_TI = std::make_shared<TextInput> (
            (SCREEN_WIDTH / 2), SCREEN_HEIGHT - 75, 16,
            [&](std::string s)
            {
                worldViewport->setFile(s);
                worldViewport->loadFile();

                worldViewport->enable();
                toolbarViewport->enable();
                mainViewport->disable();
            });


    auto regenerate_B = std::make_shared<Button> (
            toolbarViewport->getX (), toolbarViewport->getY (),
            "Regenerate World", SDL_Rect
            {20, toolbarViewport->getHeight() / 2 - 25, 280, 50}, 16,
            [&]()
            {
                std::string command = "./worldGen " + worldCommand;
                system (command.c_str ());

                worldViewport->loadFile();
            });
    auto backToMenu_B = std::make_shared<Button> (
            toolbarViewport->getX (), toolbarViewport->getY (),
            "Back To Menu", SDL_Rect
            {320, toolbarViewport->getHeight() / 2 - 25, 280, 50}, 16,
            [&]()
            {
                mainViewport->enable ();
                worldViewport->disable ();
                toolbarViewport->disable ();
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
    backToMenu_B->enable ();
    regenerate_B->enable ();

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
