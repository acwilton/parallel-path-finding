/**
 * File        : parallel-path-finding-gui.cc
 * Description : The main GUI interface for running the parallel path-finding algorithms on
 */

#include <iostream>
#include <memory>
#include <random>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Error.h"
#include "World.h"
#include "Window.h"
#include "TextInput.h"

using namespace parPath;

bool sdl_init ();

const size_t SCREEN_WIDTH = 1280;
const size_t SCREEN_HEIGHT = 960;

int main (int args, char* argv[])
{
    bool quit = !sdl_init ();

    Window window("Parallel Path Finding", SCREEN_WIDTH, SCREEN_HEIGHT);

    std::shared_ptr<Viewport> mainViewport = std::make_shared<Viewport> (
            SDL_Rect { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },
            SDL_Color {0xAA, 0x37, 0x4A});

    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    std::shared_ptr<TextInput> c;
    auto b = std::make_shared<Button> (mainViewport->getX (), mainViewport->getY (),
            "Change This Text", SDL_Rect
            { (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 50, 240, 50 }, 16,
            [&]()
            {
                c->enable();
            });

    c = std::make_shared<TextInput> (mainViewport->getX (), mainViewport->getY (),
            (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) + 25, 16,
            [&](std::string s)
            {   b->setText(s);
            });

    mainViewport->addButton (b);
    b->enable ();
    mainViewport->addButton (c);
    mainViewport->enable ();
    window.addViewport (mainViewport);

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
