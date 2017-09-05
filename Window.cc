/**
 * Window.cc
 */

#include <iostream>

#include "Window.h"

namespace parPath
{

Window::Window (std::string title, size_t width, size_t height)
        : m_title (title),
          m_width (width),
          m_height (height),
          m_keyFocus (false),
          m_mouseFocus (false),
          m_minimized (false),
          m_windowID (0),
          m_window (nullptr),
          m_renderer (nullptr)
{
}

Window::~Window ()
{
    destroyResources ();
}

void Window::focus ()
{

}

void handleEvent (SDL_Event* e)
{

}

void addViewport (uint key)
{

}

std::shared_ptr<SDL_Rect> Window::getMutableViewport (uint key)
{
    return m_viewports[key];
}

void Window::removeViewport (uint key)
{
    m_viewports.erase (key);
}

void Window::spawnWindow ()
{
    if (m_window != nullptr)
    {
        Log::logWarning ("spawnWindow() called but window is already open.");
        return;
    }

    m_window = SDL_CreateWindow (m_title.c_str (), SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);
    if (m_window == nullptr)
    {
        Log::logError (
                "Failed to initialize window. SDL_ERROR: "
                        + std::string (SDL_GetError ()) + "\n");
    }

    m_renderer = SDL_CreateRenderer (m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr)
    {
        Log::logError (
                "Failed to initialize renderer. SDL_ERROR: "
                        + std::string (SDL_GetError ()) + "\n");
    }
}

void Window::closeWindow ()
{
    if (m_window == nullptr)
    {
        Log::logWarning ("closeWindow() called but window is not open.");
        return;
    }

    destroyResources ();
}

void Window::setWidth (size_t width)
{
    m_width = width;
}

void Window::setHeight (size_t height)
{
    m_height = height;
}

size_t Window::getWidth ()
{
    return m_width;
}

size_t Window::getHeight ()
{
    return m_height;
}

bool Window::hasMouseFocus ()
{
    return m_mouseFocus;
}

bool Window::hasKeyFocus ()
{
    return m_keyFocus;
}

bool Window::isMinimized ()
{
    return m_minimized;
}

bool Window::isOpen ()
{
    return m_window != nullptr;
}

void Window::destroyResources ()
{
    SDL_DestroyWindow (m_window);
    m_window = nullptr;

    SDL_DestroyRenderer (m_renderer);
    m_renderer == nullptr;
}

} /* namespace parPath */
