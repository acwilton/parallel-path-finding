/**
 * File        : parallel-path-finding-gui.cc
 * Description : The main GUI interface for running the parallel path-finding algorithms on
 */

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#include "World.h"

using std::cout;
using std::endl;


using namespace parPath;

bool window_init (SDL_Window*& window);

bool renderer_init (SDL_Window*& window, SDL_Renderer*& renderer);

bool sdl_init (SDL_Window*& main_window, SDL_Renderer*& main_renderer);

void render_screen (SDL_Renderer* renderer, SDL_Rect* view_port);

const size_t SCREEN_WIDTH = 1280;
const size_t SCREEN_HEIGHT = 960;

int main (int args, char* argv[])
{
    // SDL variables needed
    SDL_Window* main_window = nullptr;
    SDL_Renderer* main_renderer = nullptr;
    SDL_Rect main_view_port {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    sdl_init (main_window, main_renderer);

    bool quit = false;

    SDL_Event e;

    render_screen (main_renderer, &main_view_port);

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
	render_screen (main_renderer, &main_view_port);
    }
    
    return EXIT_SUCCESS;
}

/****************************************************************************************************/
// Function Definitions

void render_screen (SDL_Renderer* renderer, SDL_Rect* view_port)
{
    SDL_SetRenderDrawColor (renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderSetViewport (renderer, view_port);
    SDL_RenderFillRect (renderer, nullptr);
    SDL_RenderPresent (renderer);
}

bool sdl_init (SDL_Window*& main_window, SDL_Renderer*& main_renderer)
{
    // SDL initialization
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
	cout << "Failed to initialized SDL. SDL_Error: " << SDL_GetError () << endl;
	return false;
    }

    if (!window_init (main_window)) { return false; }

    if (!renderer_init (main_window, main_renderer)) { return false; }

    if (TTF_Init () == -1)
    {
	cout << "SDL_ttf could not initialize. SDL_ttf Error: " << TTF_GetError() << endl;
	return false;
    }

    return true;
}

bool window_init (SDL_Window*& window)
{
    // Create Window
    window = SDL_CreateWindow ("Parallel Path-Finding", SDL_WINDOWPOS_UNDEFINED,
			       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
			       SDL_WINDOW_SHOWN);
    if (window == nullptr) {
	cout << "Failed to initialize window. SDL_ERROR: " << SDL_GetError () << endl;
	return false;
    }

    return true;
}

bool renderer_init (SDL_Window*& window, SDL_Renderer*& renderer)
{
    renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
	cout << "Failed to initialize renderer. SDL_Error: " << SDL_GetError() << endl;
    }

    return true;
}
