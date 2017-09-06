/**
 * Button.cc
 */

#include "Button.h"

namespace parPath
{

Button::Button (std::string text, SDL_Rect rect, std::function<void ()> funct,
        SDL_Color backgroundColor, SDL_Color textColor)
        : m_text (text),
          m_rect (rect),
          m_funct (funct),
          m_backgroundColor (backgroundColor),
          m_textColor (textColor)
{
}

Button::~Button ()
{
}

virtual void Button::render (SDL_Renderer* renderer)
{

}

virtual void Button::execute ()
{
    m_funct ();
}

virtual void Button::setText (std::string text)
{
    m_text = text;
}

virtual void Button::setBackgroundColor (SDL_Color color)
{
    m_backgroundColor = color;
}

virtual void Button::setTextColor (SDL_Color color)
{
    m_textColor = color;
}

virtual size_t Button::getX () const
{
    return m_rect.x;
}

virtual size_t Button::getY () const
{
    return m_rect.y;
}

virtual size_t Button::getWidth () const
{
    return m_rect.w;
}

virtual size_t Button::getHeight () const
{
    return m_rect.h;
}

virtual std::string Button::getText () const
{
    return m_text;
}

virtual SDL_Color Button::getBackgroundColor () const
{
    return m_backgroundColor;
}

virtual SDL_Color Button::getTextColor () const
{
    return m_textColor;
}

} /* namespace parPath */
