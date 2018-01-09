/**
 * Viewport.cc
 */
#include "gui/Viewport.h"

namespace pathFind
{

namespace gui
{

Viewport::Viewport (SDL_Rect rect, SDL_Color backgroundColor)
        : m_rect (rect),
          m_backgroundColor (backgroundColor),
          m_enabled (false)
{
}

Viewport::~Viewport ()
{
}

void Viewport::render (SDL_Renderer* renderer)
{
    if (isEnabled ())
    {
        SDL_RenderSetViewport (renderer, &m_rect);
        SDL_SetRenderDrawColor (renderer, m_backgroundColor.r, m_backgroundColor.g,
                m_backgroundColor.b, m_backgroundColor.a);
        SDL_RenderFillRect (renderer, nullptr);

        for (auto& t : m_texts)
        {
            t->render (renderer);
        }
        for (auto& b : m_buttons)
        {
            b->render (renderer);
        }
    }
}

void Viewport::handleEvent (SDL_Event& e)
{
    if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
        m_rect.w = e.window.data1;
        m_rect.h = e.window.data2;
    }
    if (isEnabled ())
    {
        for (auto& b : m_buttons)
        {
            b->handleEvent (e);
        }
    }
}

void Viewport::enable ()
{
    m_enabled = true;
}

void Viewport::disable ()
{
    m_enabled = false;
}

std::shared_ptr<Button> Viewport::getButton (uint pos)
{
    return m_buttons[pos];
}

std::shared_ptr<Text> Viewport::getText (uint pos)
{
    return m_texts[pos];
}

void Viewport::addButton (std::shared_ptr<Button> b)
{
    m_buttons.push_back (b);
}

void Viewport::removeButton (uint pos)
{
    m_buttons.erase (m_buttons.begin () + pos);
}

void Viewport::addText (std::shared_ptr<Text> t)
{
    m_texts.push_back (t);
}

void Viewport::removeText (uint pos)
{
    m_texts.erase (m_texts.begin () + pos);
}

void Viewport::setBackgroundColor (SDL_Color color)
{
    m_backgroundColor = color;
}

SDL_Color Viewport::getBackgroundColor () const
{
    return m_backgroundColor;
}

int Viewport::getX () const
{
    return m_rect.x;
}

int Viewport::getY () const
{
    return m_rect.y;
}

int Viewport::getWidth () const
{
    return m_rect.w;
}

int Viewport::getHeight () const
{
    return m_rect.h;
}

bool Viewport::isEnabled () const
{
    return m_enabled;
}

} /* namespace gui */
} /* namespace pathFind */
