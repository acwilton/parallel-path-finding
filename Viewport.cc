/**
 * Viewport.cc
 */

#include "Viewport.h"

namespace parPath
{

Viewport::Viewport (SDL_Rect rect, SDL_Color backgroundColor)
        : m_rect (rect),
          m_backgroundColor (backgroundColor)
{
}

Viewport::~Viewport ()
{
}

void Viewport::render (SDL_Renderer* renderer)
{
    for (auto& b : m_buttons)
    {
        b->render (renderer);
    }
}

void Viewport::handleEvent (SDL_Event* e)
{
    int mouseX, mouseY;
    SDL_GetMouseState (&mouseX, &mouseY);

    for (auto& b : m_buttons)
    {
        size_t b_globalX = getX () + b->getX ();
        size_t b_globalY = getY () + b->getY ();

        if (mouseX > b_globalX && mouseX < (b_globalX + b->getWidth ())
                && mouseY > b_globalY && mouseY < (b_globalY + b->getHeight ()))
        {
            b->execute();
            break;
        }
    }
}

std::shared_ptr<Button> Viewport::getButton (uint pos) const
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

} /* namespace parPath */
