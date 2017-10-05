/**
 * Button.cc
 */

#include "gui/Button.h"

namespace pathFind
{

namespace gui
{

Button::Button (int vp_x, int vp_y, std::string text, SDL_Rect rect,
        size_t fontSize, std::function<void ()> funct, SDL_Color backgroundColor,
        SDL_Color textColor)
        : m_viewport_x (vp_x),
          m_viewport_y (vp_y),
          m_text (text),
          m_buttonRect (rect),
          m_fontSize (fontSize),
          m_funct (funct),
          m_backgroundColor (backgroundColor),
          m_textColor (textColor),
          m_textInitialized (false),
          m_textTexture (nullptr),
          m_enabled (false)
{
}

Button::~Button ()
{
    destroyResources ();
}

void Button::render (SDL_Renderer* renderer)
{
    if (!isEnabled ())
    {
        return;
    }

    if (!m_textInitialized)
    {
        initializeTextTexture (renderer);
    }

    SDL_SetRenderDrawColor (renderer, m_backgroundColor.r, m_backgroundColor.g,
            m_backgroundColor.b, m_backgroundColor.a);
    SDL_RenderFillRect (renderer, &m_buttonRect);
    SDL_SetRenderDrawColor (renderer, m_textColor.r, m_textColor.g,
            m_textColor.b, m_textColor.a);
    SDL_RenderDrawRect (renderer, &m_buttonRect);
    SDL_RenderCopy (renderer, m_textTexture, nullptr, &m_textRect);

}

void Button::handleEvent (SDL_Event& e)
{
    if (!isEnabled ())
    {
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX, mouseY;
        SDL_GetMouseState (&mouseX, &mouseY);

        int b_globalX = getX () + m_viewport_x;
        int b_globalY = getY () + m_viewport_y;

        if (mouseX > b_globalX && mouseX < (b_globalX + getWidth ())
                && mouseY > b_globalY
                && mouseY < (b_globalY + getHeight ()))
        {
            execute ();
        }
    }
}

void Button::execute ()
{
    m_funct ();
}

void Button::enable ()
{
    m_enabled = true;
}

void Button::disable ()
{
    m_enabled = false;
}

void Button::setText (std::string text)
{
    m_text = text;
    m_textInitialized = false;
}

void Button::setFontSize (size_t fontSize)
{
    m_fontSize = fontSize;
}

void Button::setBackgroundColor (SDL_Color color)
{
    m_backgroundColor = color;
}

void Button::setTextColor (SDL_Color color)
{
    m_textColor = color;
}

int Button::getX () const
{
    return m_buttonRect.x;
}

int Button::getY () const
{
    return m_buttonRect.y;
}

int Button::getWidth () const
{
    return m_buttonRect.w;
}

int Button::getHeight () const
{
    return m_buttonRect.h;
}

std::string Button::getText () const
{
    return m_text;
}

size_t Button::getFontSize() const
{
    return m_fontSize;
}

SDL_Color Button::getBackgroundColor () const
{
    return m_backgroundColor;
}

SDL_Color Button::getTextColor () const
{
    return m_textColor;
}

bool Button::isEnabled () const
{
    return m_enabled;
}

void Button::initializeTextTexture (SDL_Renderer* renderer)
{
    destroyResources ();

    if (m_text == "")
    {
        m_text = " ";
    }

    TTF_Font* font = TTF_OpenFont ("resources/FreeSans.ttf", 128);
    SDL_Surface* textSurface = TTF_RenderText_Solid (font, m_text.c_str (),
            m_textColor);
    if (textSurface == nullptr)
    {
        Log::logError (
                "Failed to create SDL_Surface from button text: \"" + m_text
                        + "\" | SDL_ttf Error: " + TTF_GetError ());
        return;
    }
    m_textTexture = SDL_CreateTextureFromSurface (renderer, textSurface);
    SDL_FreeSurface (textSurface);

    if (m_textTexture == nullptr)
    {
        Log::logError (
                "Failed to create SDL_Texture from button text surface: \""
                        + m_text + "\" | SDL_ttf Error: " + TTF_GetError ());
        return;
    }

    int width = m_text.size() * m_fontSize;
    int height = m_fontSize * 2.25;
    int x = m_buttonRect.x + (m_buttonRect.w / 2) - (width / 2);
    int y = m_buttonRect.y + (m_buttonRect.h / 2) - (height / 2);
    SDL_Rect newTextRect = {x, y, width, height};
    m_textRect = newTextRect;

    m_textInitialized = true;
    return;
}

void Button::destroyResources ()
{
    SDL_DestroyTexture (m_textTexture);
    m_textTexture = nullptr;
}

} /* namespace gui */
} /* namespace pathFind */
