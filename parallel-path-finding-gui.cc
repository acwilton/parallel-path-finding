/**
 * File        : parallel-path-finding-gui.cc
 * Description : The main GUI interface for running the parallel path-finding algorithms on.
                 TODO:
		 #1 - Add ability to view the world files in a new SDL window (hopefully with
		        scrolling and zooming capabilities)
		 #2 - Add a generate world button that switches to a parameter window for the
		        world generation and then creates a world file from those parameters.
		        Afterwards it could then display the the world file with feature #1.
		 #3 - Create a run button that runs a certain path-finding algorithm and provides
		        stats about the run. The button will give the option of specifying a
			specific .world file or using feature #2 to generate one.
		 #4 - Ability to select multiple algorithms and then run them and create a stat
		        table using the stats from feature #3 (and maybe additional ones).
		 #5 - Add capability to view the individual paths taken by certain units. The
		        paths could be displayed by coloring the tiles traversed a certain color
			or maybe drawing lines from the center of each tile. These visualizations
			would most likely be generated on the fly because there would be too many
			possible combinations of units to visualize.
		      The screen could look similar to the following:
		      ----------------------------
		      |               | * Unit 1 |
		      |               | * Unit 2 |
		      |     Path      |    .     |
		      | Visualization |    .     |
		      |               |    .     |
		      |               | * Unit n |
		      ----------------------------
		      Where you can click on each unit you want to see the path for to switch the
		        visualization.
		 #6 - Ability to specify whether you want your units to be placed randomly around
		        the world, or have the option to manually click on the tiles you want each
			unit to spawn on for path-finding.
		 #7 - In addition to feature #4, have the ability to select multiple stored stat
		        files from previous runs to create comparison tables. That way you can
			compare the performance of algorithms against themselves in different
			settings or older interesting runs against new ones.
		 #8 - 
 */

/****************************************************************************************************/
// Global includes

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/****************************************************************************************************/
// Local includes

#include "world.h"

/****************************************************************************************************/
// Using and typedef statements

using std::cout;
using std::endl;

using namespace parPath;

/****************************************************************************************************/
// Function declaration

bool window_init (SDL_Window*& window);

bool renderer_init (SDL_Window*& window, SDL_Renderer*& renderer);

bool sdl_init (SDL_Window*& main_window, SDL_Renderer*& main_renderer);

void render_screen (SDL_Renderer* renderer, SDL_Rect* view_port);

/****************************************************************************************************/
// Constants

const size_t SCREEN_WIDTH = 1280;
const size_t SCREEN_HEIGHT = 960;

/****************************************************************************************************/
// Main

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
    if (!SDL_Init(SDL_INIT_VIDEO) < 0)
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
