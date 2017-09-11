/*
 * GraphicTile.cc
 */

#include "GraphicTile.h"

namespace parPath
{

GraphicTile::GraphicTile (World::tile_t tileData, SDL_Rect rect)
        : m_tileData (tileData),
          m_rect (rect),
          m_textRect (rect),
          m_textTexture (nullptr),
          m_textInitialized (false),
          m_textEnabled (false)
{
    if (static_cast<uint> (m_tileData.cost) == 0)
    {
        m_rectColor = {0x44, 0x44, 0x44, 0xFF};
        m_textColor = {0xFF, 0xFF, 0xFF, 0xFF};
    }
    else
    {
        m_rectColor = {0xFF, 0xFF, 0xFF, 0xFF};
        m_textColor = {0x00, 0x00, 0x00, 0xFF};
    }
}

GraphicTile::~GraphicTile ()
{
    destroyResources ();
}

void GraphicTile::setRect (SDL_Rect rect)
{
    m_rect = rect;
}

void GraphicTile::setX (int x)
{
    m_rect.x = x;
}

void GraphicTile::setY (int y)
{
    m_rect.y = y;
}

void GraphicTile::setW (int w)
{
    m_rect.w = w;
}

void GraphicTile::setH (int h)
{
    m_rect.h = h;
}

void GraphicTile::setTextEnabled (bool textEnabled)
{
    m_textEnabled = textEnabled;
}

bool GraphicTile::isTextEnabled () const
{
    return m_textEnabled;
}

SDL_Rect GraphicTile::getRect () const
{
    return m_rect;
}

World::tile_t GraphicTile::getTile () const
{
    return m_tileData;
}

void GraphicTile::render (SDL_Renderer* renderer)
{
    if (!m_textInitialized)
    {
        initializeTexture (renderer);
    }

    SDL_SetRenderDrawColor (renderer, m_rectColor.r, m_rectColor.g,
            m_rectColor.b, m_rectColor.a);
    SDL_RenderFillRect (renderer, &m_rect);
    SDL_SetRenderDrawColor (renderer, m_textColor.r, m_textColor.g,
            m_textColor.b, m_textColor.a);
    SDL_RenderDrawRect (renderer, &m_rect);
    SDL_RenderCopy (renderer, m_textTexture, nullptr, &m_textRect);
}

void GraphicTile::initializeTexture (SDL_Renderer* renderer)
{
    destroyResources ();
    std::string text = std::to_string (static_cast<int> (m_tileData.cost));

    TTF_Font* font = TTF_OpenFont ("FreeSans.ttf", 128);
    SDL_Surface* textSurface = TTF_RenderText_Solid (font,
            text.c_str(),
            m_textColor);
    if (textSurface == nullptr)
    {
        Log::logError (
                "Failed to create SDL_Surface from tile data: \"" + text
                        + "\" | SDL_ttf Error: " + TTF_GetError ());
        return;
    }
    m_textTexture = SDL_CreateTextureFromSurface (renderer, textSurface);
    SDL_FreeSurface (textSurface);

    if (m_textTexture == nullptr)
    {
        Log::logError (
                "Failed to create SDL_Texture from tile data text surface: \""
                        + text + "\" | SDL_ttf Error: " + TTF_GetError ());
        return;
    }

    m_textRect = {m_rect.x + 2, m_rect.y + 2, m_rect.w - 4, m_rect.h - 4};

    m_textInitialized = true;
    return;
}

void GraphicTile::destroyResources ()
{
    SDL_DestroyTexture (m_textTexture);
    m_textTexture = nullptr;
}

} /* namespace parPath */
