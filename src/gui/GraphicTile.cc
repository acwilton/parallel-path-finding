/*
 * GraphicTile.cc
 */

#include "gui/GraphicTile.h"

namespace pathFind
{

namespace gui
{

GraphicTile::GraphicTile (World::tile_t tileData, SDL_Rect rect)
        : m_tileData (tileData),
          m_rect (rect),
          m_textRect (rect),
          m_textEnabled (false)
{
    if (static_cast<uint> (m_tileData.cost) == 0)
    {
        m_rectColor = {0x44, 0x44, 0x44, 0xFF};
        m_textColor = {0xFF, 0xFF, 0xFF, 0xFF};
    }
    else
    {
        m_rectColor = DEFAULT_COLOR;
        m_textColor = {0x00, 0x00, 0x00, 0xFF};
    }
}

GraphicTile::~GraphicTile ()
{
}

void GraphicTile::setRect (SDL_Rect rect)
{
    m_rect = rect;
    updateTextRect ();
}

void GraphicTile::setX (int x)
{
    m_rect.x = x;
    updateTextRect ();
}

void GraphicTile::setY (int y)
{
    m_rect.y = y;
    updateTextRect ();
}

void GraphicTile::setW (int w)
{
    m_rect.w = w;
    updateTextRect ();
}

void GraphicTile::setH (int h)
{
    m_rect.h = h;
    updateTextRect ();
}

void GraphicTile::setTextEnabled (bool textEnabled)
{
    if (m_textEnabled != textEnabled)
    {
        updateTextRect ();
    }
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

void GraphicTile::setRectColor(SDL_Color color)
{
    m_rectColor = color;
}

void GraphicTile::render (SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_SetRenderDrawColor (renderer, m_rectColor.r, m_rectColor.g,
            m_rectColor.b, m_rectColor.a);
    SDL_RenderFillRect (renderer, &m_rect);
    SDL_SetRenderDrawColor (renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawRect (renderer, &m_rect);

    if (m_textEnabled && static_cast<int>(m_tileData.cost) != 0)
    {
        SDL_RenderCopy (renderer, texture, nullptr, &m_textRect);
    }
}

void GraphicTile::updateTextRect ()
{
    m_textRect = {m_rect.x + 2, m_rect.y + 2, m_rect.w - 4, m_rect.h - 4};
}

} /* namespace gui */
} /* namespace pathFind */
