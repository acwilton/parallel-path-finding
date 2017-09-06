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

void Window::handleEvent (SDL_Event* e)
{

}

void Window::render ()
{
    for (auto& vp : m_viewports)
    {
        vp->render(m_renderer);
    }
}

std::shared_ptr<Viewport> Window::getViewport (uint pos) const
{
    return m_viewports[pos];
}

void Window::addViewport (std::shared_ptr<Viewport> vp)
{
    m_viewports.push_back (vp);
}

void Window::removeViewport (uint pos)
{
    m_viewports.erase (m_viewports.begin() + pos);
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

    m_windowID = SDL_GetWindowID (m_window);
    m_minimized = false;
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
void Window::setTitle (std::string title)
{
    m_title = title;
}

size_t Window::getWidth () const
{
    return m_width;
}

size_t Window::getHeight () const
{
    return m_height;
}

std::string Window::getTitle () const
{
    return m_title;
}

bool Window::hasMouseFocus () const
{
    return m_mouseFocus;
}

bool Window::hasKeyFocus () const
{
    return m_keyFocus;
}

bool Window::isMinimized () const
{
    return m_minimized;
}

bool Window::isOpen () const
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
