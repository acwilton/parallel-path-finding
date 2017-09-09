/**
 * Viewport.cc
 */
#include "Viewport.h"

namespace parPath
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
    if (m_enabled)
    {
        SDL_RenderSetViewport (renderer, &m_rect);
        SDL_SetRenderDrawColor (renderer, m_backgroundColor.r, m_backgroundColor.g,
                m_backgroundColor.b, m_backgroundColor.a);
        SDL_RenderFillRect (renderer, nullptr);
        for (auto& b : m_buttons)
        {
            b->render (renderer);
        }
    }
}

void Viewport::handleEvent (SDL_Event& e)
{
    if (m_enabled)
    {
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouseX, mouseY;
            SDL_GetMouseState (&mouseX, &mouseY);

            for (auto& b : m_buttons)
            {
                size_t b_globalX = getX () + b->getX ();
                size_t b_globalY = getY () + b->getY ();

                if (mouseX > b_globalX && mouseX < (b_globalX + b->getWidth ())
                        && mouseY > b_globalY
                        && mouseY < (b_globalY + b->getHeight ()))
                {
                    b->execute ();
                    break;
                }
            }
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

void Viewport::addButton (std::shared_ptr<Button> b)
{
    m_buttons.push_back (b);
}

void Viewport::removeButton (uint pos)
{
    m_buttons.erase (m_buttons.begin () + pos);
}

void Viewport::setBackgroundColor (SDL_Color color)
{
    m_backgroundColor = color;
}

SDL_Color Viewport::getBackgroundColor () const
{
    return m_backgroundColor;
}

size_t Viewport::getX () const
{
    return m_rect.x;
}

size_t Viewport::getY () const
{
    return m_rect.y;
}

size_t Viewport::getWidth () const
{
    return m_rect.w;
}

size_t Viewport::getHeight () const
{
    return m_rect.h;
}

bool Viewport::isEnabled () const
{
    return m_enabled;
}

} /* namespace parPath */
