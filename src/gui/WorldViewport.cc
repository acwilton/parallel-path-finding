/*
 * WorldViewport.cc
 */

#include <fstream>

#include "gui/WorldViewport.h"

namespace pathFind
{

namespace gui
{

const uint MIN_TILE_SCALE = 5;
const uint MAX_TILE_SCALE = 75;
const uint DEFAULT_TILE_SCALE = 10;

WorldViewport::WorldViewport (SDL_Rect rect, SDL_Color backgroundColor)
        : Viewport (rect, backgroundColor),
          m_worldFileName (" "),
          m_cameraX (0),
          m_cameraY (0),
          m_tileScale (DEFAULT_TILE_SCALE),
          m_textEnabled (false),
          m_textInitialized (false)
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
        for (uint y = m_cameraY; y < m_gTiles.size (); ++y)
        {
            for (uint x = m_cameraX; x < m_gTiles[y].size (); ++x)
            {

                if (m_gTiles[y][x].getRect ().x > getWidth ()
                    || m_gTiles[y][x].getRect ().y > getHeight ())
                {
                    break;
                }

                m_gTiles[y][x].render (renderer,
                        m_textTextures[static_cast<int>(m_gTiles[y][x].getTile().cost)]);
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
                if (getCameraOppY () + moveSpeed <= m_gTiles.size ())
                {
                    setCameraY (getCameraY () + moveSpeed);
                }
                else
                {
                    setCameraY (m_gTiles.size () - (m_rect.h / m_tileScale));
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
                if (getCameraOppX () + moveSpeed < m_gTiles.back ().size ())
                {
                    setCameraX (getCameraX () + moveSpeed);
                }
                else
                {
                    setCameraX (m_gTiles.back ().size () - (m_rect.w / m_tileScale));
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

void WorldViewport::setFile (std::string worldFileName)
{
    m_worldFileName = worldFileName;
}

void WorldViewport::loadFile ()
{
    m_gTiles.clear();
    m_cameraX = 0;
    m_cameraY = 0;

    std::string trueWorldName = "worlds/" + m_worldFileName + ".world";
    Log::logInfo("Created world: " + trueWorldName);
    std::ifstream worldFile(trueWorldName, std::ifstream::in | std::ifstream::binary);
    if (!worldFile)
    {
        Log::logError ("WorldViewport either could not find or could not open "
                + trueWorldName);
        return;
    }

    World world;
    worldFile >> world;

    m_gTiles.resize(world.getHeight ());

    for (uint y = 0; y < m_gTiles.size (); ++y)
    {
        m_gTiles[y].reserve(world.getWidth ());
        for (uint x = 0; x < world.getWidth (); ++x)
        {
            m_gTiles[y].emplace_back (world(y, x),
                    SDL_Rect {static_cast<int> (x * m_tileScale),
                              static_cast<int> (y * m_tileScale),
                              static_cast<int> (m_tileScale),
                              static_cast<int> (m_tileScale)});
        }
    }
}

void WorldViewport::setCameraX (int x)
{
    if (x < 0)
    {
        x = 0;
    }
    m_cameraX = x;
    for (uint y = 0; y < m_gTiles.size (); ++y)
    {
        for (uint x = 0; x < m_gTiles[y].size (); ++x)
        {
            m_gTiles[y][x].setX((x - m_cameraX) * m_tileScale);
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
    for (uint y = 0; y < m_gTiles.size (); ++y)
    {
        for (uint x = 0; x < m_gTiles[y].size (); ++x)
        {
            m_gTiles[y][x].setY((y - m_cameraY) * m_tileScale);
        }
    }
}

void WorldViewport::setTileScale (int scale)
{
    m_tileScale = scale;

    if (getCameraOppY () > m_gTiles.size ())
    {
        setCameraY (getCameraY () - (getCameraOppY () - m_gTiles.size ()));
    }
    if (getCameraOppX () > m_gTiles.back ().size ())
    {
        setCameraX (getCameraX () - (getCameraOppX () - m_gTiles.back ().size ()));
    }

    for (uint y = 0; y < m_gTiles.size (); ++y)
    {
        for (uint x = 0; x < m_gTiles[y].size (); ++x)
        {
            m_gTiles[y][x].setW (m_tileScale);
            m_gTiles[y][x].setH (m_tileScale);
            m_gTiles[y][x].setX((x - m_cameraX) * m_tileScale);
            m_gTiles[y][x].setY((y - m_cameraY) * m_tileScale);
        }
    }
}

void WorldViewport::setTextEnabled (bool textEnabled)
{
    m_textEnabled = textEnabled;
    for (auto& row : m_gTiles)
    {
        for (auto& t : row)
        {
            t.setTextEnabled(m_textEnabled);
        }
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
