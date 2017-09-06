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
    // SDL variables needed
    Window mainWindow ("Parallel Path Finding", SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Renderer* main_renderer = nullptr;
    Viewport mainViewport (
    { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });

    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    SDL_Rect xr;
    auto b = std::make_shared<Button>("Change Color",
    SDL_Rect{ 10,10, 80, 30 });

    /*auto b = std::make_shared<Button>("Change Color",
    { (SCREEN_WIDTH / 2) - 40, (SCREEN_HEIGHT / 2) - 15, 80, 30 }, [&]()
    {   mainViewport.setBackgroundColor(
                {   static_cast<uchar>(gen() % 255),
                    static_cast<uchar>(gen() % 255),
                    static_cast<uchar>(gen() % 255), 0xFF}
        );
    });*/

    bool quit = !sdl_init ();

    SDL_Event e;

    mainWindow.spawnWindow ();

    mainWindow.render ();

    // application loop
    while (!quit)
    {
        while (SDL_PollEvent (&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        mainWindow.render ();
    }

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
