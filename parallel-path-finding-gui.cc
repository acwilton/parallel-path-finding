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

using namespace parPath;

bool sdl_init ();

const size_t SCREEN_WIDTH = 1280;
const size_t SCREEN_HEIGHT = 960;

int main (int args, char* argv[])
{
    bool quit = !sdl_init ();

    Window window("Parallel Path Finding", SCREEN_WIDTH, SCREEN_HEIGHT);

    std::shared_ptr<Viewport> mainViewport = std::make_shared<Viewport> (
            SDL_Rect
            { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
    std::shared_ptr<Viewport> tbViewport = std::make_shared<Viewport> (
            SDL_Rect
            { 0, SCREEN_HEIGHT - 240, SCREEN_WIDTH, 240});

    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    auto b = std::make_shared<Button> ("Change Color 1", SDL_Rect
    { (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 125, 240, 50 }, 16, [&]()
    {   mainViewport->setBackgroundColor(
        {
            static_cast<uchar>(gen() % 255),
            static_cast<uchar>(gen() % 255),
            static_cast<uchar>(gen() % 255), 0xFF
        });
    });

    auto c = std::make_shared<Button> ("Change Color 2", SDL_Rect
    { (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) - 50, 240, 50 }, 16, [&]()
    {   tbViewport->setBackgroundColor(
        {
            static_cast<uchar>(gen() % 255),
            static_cast<uchar>(gen() % 255),
            static_cast<uchar>(gen() % 255), 0xFF
        });
    });

    auto d = std::make_shared<Button> ("Toggle Viewport", SDL_Rect
    { (SCREEN_WIDTH / 2) - 120, (SCREEN_HEIGHT / 2) + 25, 240, 50 }, 16, [&]()
    {
        tbViewport->isEnabled() ? tbViewport->disable() : tbViewport->enable();
    });

    auto f = std::make_shared<Button> ("Toggle Viewport", SDL_Rect
    { (SCREEN_WIDTH / 2) - 120, 0, 240, 50 }, 16, [&]()
    {
        mainViewport->isEnabled() ? mainViewport->disable() : mainViewport->enable();
    });

    mainViewport->addButton (b);
    mainViewport->addButton (c);
    mainViewport->addButton (d);
    tbViewport->addButton (f);
    tbViewport->enable();
    window.addViewport (mainViewport);
    window.addViewport (tbViewport);

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
