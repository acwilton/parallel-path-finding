/**
 * Window.cc
 */

#include <iostream>

#include "Window.h"

namespace parPath
{

Window::Window(std::string title, size_t width, size_t height)
{
    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (m_window == nullptr)
    {
        Log::logError("Failed to initialize window. SDL_ERROR: " + std::string (SDL_GetError()) + "\n");
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr)
    {
        Log::logError("Failed to initialize renderer. SDL_ERROR: " + std::string (SDL_GetError()) + "\n");
    }
}

Window::~Window()
{
    // TODO Auto-generated destructor stub
}

} /* namespace parPath */
