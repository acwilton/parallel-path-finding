/**
 * Button.cc
 */

#include "Button.h"
namespace parPath
{

Button::Button (std::string text, SDL_Rect rect, size_t fontSize,
        std::function<void ()> funct, SDL_Color backgroundColor,
        SDL_Color textColor)
        : m_text (text),
          m_buttonRect (rect),
          m_fontSize (fontSize),
          m_funct (funct),
          m_backgroundColor (backgroundColor),
          m_textColor (textColor),
          m_textInitialized (false),
          m_textTexture (nullptr)
{
}

Button::~Button ()
{
    destroyResources ();
}

void Button::render (SDL_Renderer* renderer)
{
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

void Button::execute ()
{
    m_funct ();
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

size_t Button::getX () const
{
    return m_buttonRect.x;
}

size_t Button::getY () const
{
    return m_buttonRect.y;
}

size_t Button::getWidth () const
{
    return m_buttonRect.w;
}

size_t Button::getHeight () const
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

void Button::initializeTextTexture (SDL_Renderer* renderer)
{
    destroyResources ();
    m_textRect = {0, 0, 0, 0};

    if (m_text == "")
    {
        m_textInitialized = true;
        return;
    }

    TTF_Font* font = TTF_OpenFont ("FreeSans.ttf", 128);
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

} /* namespace parPath */
