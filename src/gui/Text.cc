/**
 * Button.cc
 */

#include <iostream>

#include "gui/Text.h"

namespace pathFind
{

namespace gui
{

Text::Text (int vp_x, int vp_y, std::string text, SDL_Rect rect,
        size_t fontSize, SDL_Color textColor)
        : m_viewport_x (vp_x),
          m_viewport_y (vp_y),
          m_text (text),
          m_fontSize (fontSize),
          m_textColor (textColor),
          m_textInitialized (false),
          m_textTexture (nullptr),
          m_textRect (rect),
          m_enabled (false)
{
}

Text::~Text ()
{
    destroyResources ();
}

void Text::render (SDL_Renderer* renderer)
{
    if (!isEnabled ())
    {
        return;
    }

    if (!m_textInitialized)
    {
        initializeTextTexture (renderer);
    }

    SDL_SetRenderDrawColor (renderer, m_textColor.r, m_textColor.g,
            m_textColor.b, m_textColor.a);
    SDL_RenderCopy (renderer, m_textTexture, nullptr, &m_textRect);
}

void Text::enable ()
{
    m_enabled = true;
}

void Text::disable ()
{
    m_enabled = false;
}

void Text::setText (std::string text)
{
    m_text = text;
    m_textInitialized = false;
}

void Text::setFontSize (size_t fontSize)
{
    m_fontSize = fontSize;
}

void Text::setTextColor (SDL_Color color)
{
    m_textColor = color;
}

int Text::getX () const
{
    return m_textRect.x;
}

int Text::getY () const
{
    return m_textRect.y;
}

int Text::getWidth () const
{
    return m_textRect.w;
}

int Text::getHeight () const
{
    return m_textRect.h;
}

std::string Text::getText () const
{
    return m_text;
}

size_t Text::getFontSize() const
{
    return m_fontSize;
}

SDL_Color Text::getTextColor () const
{
    return m_textColor;
}

bool Text::isEnabled () const
{
    return m_enabled;
}

void Text::initializeTextTexture (SDL_Renderer* renderer)
{
    destroyResources ();

    if (m_text == "")
    {
        m_text = " ";
    }

    TTF_Font* font = TTF_OpenFont ("../resources/FreeSans.ttf", 128);
    if (font == nullptr)
    {
        Log::logError (
                std::string ("Failed to open font: \"../resources/FreeSans.ttf\" | SDL_ttf Error: ")
                + TTF_GetError ());
        return;
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid (font, m_text.c_str (),
            m_textColor);
    TTF_CloseFont(font);
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
    int x = m_textRect.x + (m_textRect.w / 2) - (width / 2);
    int y = m_textRect.y + (m_textRect.h / 2) - (height / 2);
    SDL_Rect newTextRect = {x, y, width, height};
    m_textRect = newTextRect;

    m_textInitialized = true;
    return;
}

void Text::destroyResources ()
{
    SDL_DestroyTexture (m_textTexture);
    m_textTexture = nullptr;
}

} /* namespace gui */
} /* namespace pathFind */
