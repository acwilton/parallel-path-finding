/*
 * WorldViewport.cc
 */

#include <iostream>
#include <fstream>

#include "gui/WorldViewport.h"
#include "common/Results.h"

namespace pathFind
{

namespace gui
{

const uint MIN_TILE_SCALE = 5;
const uint MAX_TILE_SCALE = 75;
const uint DEFAULT_TILE_SCALE = 10;

const SDL_Color SELECT_COLOR = {0xFF, 0x77, 0x00, 0xFF};
const SDL_Color DIJKSTRA_COLOR = {0xFF, 0xF4, 0x7F, 0xFF};

WorldViewport::WorldViewport (SDL_Rect rect, SDL_Color backgroundColor)
        : Viewport (rect, backgroundColor),
          m_worldName (" "),
          m_worldWidth (0),
          m_worldHeight (0),
          m_cameraX (0),
          m_cameraY (0),
          m_tileScale (DEFAULT_TILE_SCALE),
          m_textEnabled (false),
          m_textInitialized (false),
          m_resultsEnabled (false)
{
    for (uint i = 0; i < 255; ++i)
    {
        m_textTextures[i] = nullptr;
    }
}

WorldViewport::~WorldViewport ()
{
    destroyResources ();
}

void WorldViewport::render (SDL_Renderer* renderer)
{
    if (isEnabled ())
    {
        if (!m_textInitialized){
            initializeTextures (renderer);
        }
        SDL_RenderSetViewport (renderer, &m_rect);
        SDL_SetRenderDrawColor (renderer, m_backgroundColor.r, m_backgroundColor.g,
                        m_backgroundColor.b, m_backgroundColor.a);
        SDL_RenderFillRect (renderer, nullptr);
        for (uint y = m_cameraY; y < m_worldHeight; ++y)
        {
            for (uint x = m_cameraX; x < m_worldWidth; ++x)
            {

                if (m_gTiles[getIndex (x, y)].getRect ().x > getWidth ()
                    || m_gTiles[getIndex (x, y)].getRect ().y > getHeight ())
                {
                    break;
                }

                m_gTiles[getIndex (x, y)].render (renderer,
                    m_textTextures[static_cast<int>(m_gTiles[getIndex (x, y)].getTile().cost)]);
            }
        }
    }
}

void WorldViewport::handleEvent (SDL_Event& e)
{
    if  (isEnabled ())
    {
        if (e.type == SDL_KEYDOWN)
        {
            uint moveSpeed = 1;
            uint scaleSpeed = 1;
            if ((SDL_GetModState () & KMOD_CTRL))
            {
                moveSpeed = 5;
                scaleSpeed = 2;
            }
            switch (e.key.keysym.sym)
            {
            case SDLK_UP:
            case SDLK_w:
                if (getCameraY () - moveSpeed >= 0)
                {
                    setCameraY (getCameraY () - moveSpeed);
                }
                else
                {
                    setCameraY (0);
                }
                break;
            case SDLK_DOWN:
            case SDLK_s:
                if (getCameraOppY () + moveSpeed <= m_worldHeight)
                {
                    setCameraY (getCameraY () + moveSpeed);
                }
                else
                {
                    setCameraY (m_worldHeight - (m_rect.h / m_tileScale));
                }
                break;
            case SDLK_LEFT:
            case SDLK_a:
                if (getCameraX () - moveSpeed > 0)
                {
                    setCameraX (getCameraX () - moveSpeed);
                }
                else
                {
                    setCameraX (0);
                }
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                if (getCameraOppX () + moveSpeed < m_worldWidth)
                {
                    setCameraX (getCameraX () + moveSpeed);
                }
                else
                {
                    setCameraX (m_worldWidth - (m_rect.w / m_tileScale));
                }
                break;
            case SDLK_PLUS:
            case SDLK_EQUALS:
                if (m_tileScale + scaleSpeed < MAX_TILE_SCALE)
                {
                    setTileScale (getTileScale () + scaleSpeed);
                }
                else
                {
                    setTileScale (MAX_TILE_SCALE);
                }
                break;
            case SDLK_MINUS:
                if (m_tileScale - scaleSpeed > MIN_TILE_SCALE)
                {
                    setTileScale (getTileScale () - scaleSpeed);
                }
                else
                {
                    setTileScale (MIN_TILE_SCALE);
                }
                break;
            case SDLK_t:
                setTextEnabled (!m_textEnabled);
                break;
            }
        }
    }
}

void WorldViewport::setWorld (const std::string& worldName)
{
    m_worldName = worldName;
}

void WorldViewport::loadWorld ()
{
    m_gTiles.clear();
    m_cameraX = 0;
    m_cameraY = 0;

    std::string worldFileName = "worlds/" + m_worldName + ".world";
    Log::logInfo("Created world: " + worldFileName);
    std::ifstream worldFile(worldFileName, std::ifstream::in | std::ifstream::binary);
    if (!worldFile)
    {
        Log::logError ("WorldViewport either could not find or could not open "
                + worldFileName);
        return;
    }

    World world;
    worldFile >> world;

    m_worldWidth = world.getWidth ();
    m_worldHeight= world.getHeight ();
    m_gTiles.reserve(m_worldWidth * m_worldHeight);

    for (uint y = 0; y < m_worldHeight; ++y)
    {
        for (uint x = 0; x < m_worldWidth; ++x)
        {
            m_gTiles.emplace_back (world(x, y),
                    SDL_Rect {static_cast<int> (x * m_tileScale),
                              static_cast<int> (y * m_tileScale),
                              static_cast<int> (m_tileScale),
                              static_cast<int> (m_tileScale)});
            m_gTiles.back().setTextEnabled(m_textEnabled);
        }
    }
}

void WorldViewport::loadResults (const Point& start, const Point& end,
                                const std::string& algName)
{
    m_results.clear();
    readResults (m_results, start, end, m_worldName, algName);
}

void WorldViewport::setResultsEnabled (bool resultsEnabled)
{
    SDL_Color color = resultsEnabled ? DIJKSTRA_COLOR : DEFAULT_COLOR;
    for (auto& r : m_results)
    {
        m_gTiles[getIndex (r.x, r.y)].setRectColor(color);
    }
}

void WorldViewport::setCameraX (int x)
{
    if (x < 0)
    {
        x = 0;
    }
    m_cameraX = x;
    for (uint y = 0; y < m_worldHeight; ++y)
    {
        for (uint x = 0; x < m_worldWidth; ++x)
        {
            m_gTiles[getIndex (x, y)].setX((x - m_cameraX) * m_tileScale);
        }
    }
}

void WorldViewport::setCameraY (int y)
{
    if (y < 0)
    {
        y = 0;
    }
    m_cameraY = y;
    for (uint y = 0; y < m_worldHeight; ++y)
    {
        for (uint x = 0; x < m_worldWidth; ++x)
        {
            m_gTiles[getIndex (x, y)].setY((y - m_cameraY) * m_tileScale);
        }
    }
}

void WorldViewport::setTileScale (int scale)
{
    m_tileScale = scale;

    if (getCameraOppY () > m_worldHeight)
    {
        setCameraY (getCameraY () - (getCameraOppY () - m_worldHeight));
    }
    if (getCameraOppX () > m_worldWidth)
    {
        setCameraX (getCameraX () - (getCameraOppX () - m_worldWidth));
    }

    for (uint y = 0; y < m_worldHeight; ++y)
    {
        for (uint x = 0; x < m_worldWidth; ++x)
        {
            m_gTiles[getIndex (x, y)].setW (m_tileScale);
            m_gTiles[getIndex (x, y)].setH (m_tileScale);
            m_gTiles[getIndex (x, y)].setX((x - m_cameraX) * m_tileScale);
            m_gTiles[getIndex (x, y)].setY((y - m_cameraY) * m_tileScale);
        }
    }
}

void WorldViewport::setTextEnabled (bool textEnabled)
{
    m_textEnabled = textEnabled;
    for (auto& t : m_gTiles)
    {
        t.setTextEnabled(m_textEnabled);
    }
}

uint WorldViewport::getCameraX () const
{
    return m_cameraX;
}

uint WorldViewport::getCameraY () const
{
    return m_cameraY;
}

uint WorldViewport::getTileScale () const
{
    return m_tileScale;
}

uint WorldViewport::getIndex (uint x, uint y) const
{
    return (m_worldWidth * y) + x;
}

uint WorldViewport::getCameraOppX () const
{
    return (m_rect.w / m_tileScale) + m_cameraX;
}

uint WorldViewport::getCameraOppY () const
{
    return (m_rect.h / m_tileScale) + m_cameraY;
}

void WorldViewport::initializeTextures (SDL_Renderer* renderer)
{
    destroyResources ();
    for (uint i = 0; i < 256; ++i)
    {
        std::string text = std::to_string (i);

        TTF_Font* font = TTF_OpenFont ("resources/FreeSans.ttf", 128);
        SDL_Surface* textSurface = TTF_RenderText_Solid (font,
                text.c_str(),
                {0x00,0x00,0x00});
        if (textSurface == nullptr)
        {
            Log::logError (
                    "Failed to create SDL_Surface from tile data: \"" + text
                            + "\" | SDL_ttf Error: " + TTF_GetError ());
            return;
        }
        m_textTextures[i] = SDL_CreateTextureFromSurface (renderer, textSurface);
        SDL_FreeSurface (textSurface);

        if (m_textTextures[i] == nullptr)
        {
            Log::logError (
                    "Failed to create SDL_Texture from tile data text surface: \""
                            + text + "\" | SDL_ttf Error: " + TTF_GetError ());
            return;
        }
    }

    m_textInitialized = true;

    return;
}

void WorldViewport::destroyResources ()
{
    for (uint i = 0; i < 256; ++i)
    {
        SDL_DestroyTexture (m_textTextures[i]);
        m_textTextures[i] = nullptr;
    }
}

} /* namespace gui */
} /* namespace pathFind */
