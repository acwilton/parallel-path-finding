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
const SDL_Color ASTAR_COLOR = {0x82, 0xFF, 0x86, 0xFF};

WorldViewport::WorldViewport (SDL_Rect rect, SDL_Color backgroundColor)
        : Viewport (rect, backgroundColor),
          m_mode (VIEW),
          m_worldName (" "),
          m_worldWidth (0),
          m_worldHeight (0),
          m_cameraX (0),
          m_cameraY (0),
          m_tileScale (DEFAULT_TILE_SCALE),
          m_textEnabled (false),
          m_textInitialized (false),
          m_resultsEnabled (false),
          m_showEndPoints (false),
          m_start (0, 0),
          m_end (0, 0),
          m_startTexture (nullptr),
          m_endTexture (nullptr)
{
    for (uint i = 0; i < 256; ++i)
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
                GraphicTile& t = m_gTiles[getIndex (x, y)];

                if (t.getRect ().x > getWidth ()
                    || t.getRect ().y > getHeight ())
                {
                    break;
                }

                SDL_Texture* renderTexture = m_textTextures[t.getTile().cost];
                if (m_showEndPoints)
                {
                    if (m_start.x == x && m_start.y == y)
                    {
                        renderTexture = m_startTexture;
                    }
                    else if (m_end.x == x && m_end.y == y)
                    {
                        renderTexture = m_endTexture;
                    }
                }
                t.render (renderer, renderTexture);
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
            case SDLK_LALT:
                if (m_mode == VIEW)
                {
                    m_showEndPoints = true;
                }
                break;
            case SDLK_d:
                if (m_mode == SELECT && !isNull (m_start) && !isNull (m_end))
                {
                    runAndLoadPathFinding ("dijkstra");
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
                break;
            case SDLK_a:
                if (m_mode == SELECT && !isNull (m_start) && !isNull (m_end))
                {
                    runAndLoadPathFinding ("aStar");
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
            }
        }
        else if (e.type == SDL_KEYUP)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_LALT:
                if (m_mode == VIEW)
                {
                    m_showEndPoints = false;
                }
            }
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouseX = e.button.x - getX ();
            int mouseY = e.button.y - getY ();

            if (mouseX > 0 && mouseX < getWidth () &&
                mouseY > 0 && mouseY < getHeight ())
            {
                if (m_mode == VIEW)
                {
                    m_start = trySelectTile (mouseX, mouseY);
                    if (!isNull (m_start))
                    {
                        setMode (SELECT);
                        GraphicTile& t = m_gTiles[getIndex (m_start.x, m_start.y)];
                        m_startPrevColor = t.getRectColor ();
                        t.setRectColor (SELECT_COLOR);

                        // Reset m_end
                        m_end.x = m_worldWidth;
                        m_end.y = m_worldHeight;
                    }
                }
                else if (m_mode == SELECT)
                {
                    if (isNull (m_end))
                    {
                        m_end = trySelectTile (mouseX, mouseY);
                        if (!isNull (m_end))
                        {
                            GraphicTile& t = m_gTiles[getIndex (m_end.x, m_end.y)];
                            m_endPrevColor = t.getRectColor();
                            t.setRectColor(SELECT_COLOR);
                        }
                    }
                    else
                    {
                        setMode (VIEW);
                        m_gTiles[getIndex (m_start.x, m_start.y)].setRectColor(m_startPrevColor);
                        m_gTiles[getIndex (m_end.x, m_end.y)].setRectColor(m_endPrevColor);
                        resetEndPoints ();
                        if (!m_results.empty())
                        {
                            m_start = m_results.front();
                            m_end = m_results.back();
                        }
                    }
                }
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
    m_results.clear();
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

    resetEndPoints ();

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

void WorldViewport::runAndLoadPathFinding (const std::string& algorithm)
{
    m_currentAlgorithm = algorithm;
    setMode (VIEW);
    std::string command;
    command = "./" + m_currentAlgorithm + " " + m_worldName + " " +
            std::to_string (m_start.x) + " " +
            std::to_string (m_start.y) + " " +
            std::to_string (m_end.x) + " " +
            std::to_string (m_end.y);
    system (command.c_str ());
    loadResults (m_start, m_end, m_currentAlgorithm);
    setResultsEnabled (true);
}

void WorldViewport::loadResults (const Point& start, const Point& end,
                                const std::string& algName)
{
    m_currentAlgorithm = algName;
    setResultsEnabled (false);
    m_results.clear();
    readResults (m_results, start, end, m_worldName, algName);
    if (m_mode == VIEW)
    {
        if (!m_results.empty())
        {
            m_start = m_results.front();
            m_end = m_results.back();
        }
    }
}

void WorldViewport::setResultsEnabled (bool resultsEnabled)
{
    m_resultsEnabled = resultsEnabled;
    SDL_Color color = m_resultsEnabled ? getAlgorithmColor () : DEFAULT_COLOR;
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
void WorldViewport::setShowEndPoints(bool showEndPoints)
{
    m_showEndPoints = showEndPoints;
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

void WorldViewport::setMode (Mode mode)
{
    m_mode = mode;
    switch (m_mode)
    {
    case VIEW:
        m_showEndPoints = false;
        break;
    case SELECT:
        m_showEndPoints = true;
        break;
    }
}

Point WorldViewport::trySelectTile (int mouseX, int mouseY)
{
    Point tile (m_worldWidth, m_worldHeight);

    uint tileX = m_cameraX + (static_cast<uint> (mouseX) / m_tileScale);
    uint tileY = m_cameraY + (static_cast<uint> (mouseY) / m_tileScale);
    if (tileX < m_worldWidth && tileY < m_worldHeight)
    {
        GraphicTile& t = m_gTiles[getIndex (tileX, tileY)];
        if (t.getTile().cost != 0)
        {
            tile.x = tileX;
            tile.y = tileY;
        }
    }
    return tile;
}

SDL_Color WorldViewport::getAlgorithmColor () const
{
    if (m_currentAlgorithm == "dijkstra")
    {
        return DIJKSTRA_COLOR;
    }
    else if (m_currentAlgorithm == "aStar")
    {
        return ASTAR_COLOR;
    }

    return DEFAULT_COLOR;
}

uint WorldViewport::getIndex (uint x, uint y) const
{
    return (m_worldWidth * y) + x;
}

bool WorldViewport::isNull(const Point& p) const
{
    return (p.x == m_worldWidth && p.y == m_worldHeight);
}

uint WorldViewport::getCameraOppX () const
{
    return (m_rect.w / m_tileScale) + m_cameraX;
}

uint WorldViewport::getCameraOppY () const
{
    return (m_rect.h / m_tileScale) + m_cameraY;
}

void WorldViewport::resetEndPoints()
{
    // a start/end 1 past the last tile's x and y is considered a null value
    m_start.x = m_end.x = m_worldWidth;
    m_start.y = m_end.y = m_worldHeight;
}

void WorldViewport::initializeTextures (SDL_Renderer* renderer)
{
    destroyResources ();
    for (uint i = 0; i < 256; ++i)
    {
        initializeTexture (renderer, m_textTextures[i], std::to_string (i));
    }
    initializeTexture (renderer, m_startTexture, "S");
    initializeTexture (renderer, m_endTexture, "E");

    m_textInitialized = true;

    return;
}

void WorldViewport::initializeTexture(SDL_Renderer* renderer, SDL_Texture*& texture,
                                      const std::string& text)
{
    TTF_Font* font = TTF_OpenFont ("resources/FreeSans.ttf", 128);
    SDL_Surface* textSurface = TTF_RenderText_Solid (font, text.c_str(),
                                                     {0x00,0x00,0x00});
    if (textSurface == nullptr)
    {
        Log::logError (
                "Failed to create SDL_Surface from tile data: \"" + text
                        + "\" | SDL_ttf Error: " + TTF_GetError ());
        return;
    }
    texture = SDL_CreateTextureFromSurface (renderer, textSurface);
    SDL_FreeSurface (textSurface);
    if (texture == nullptr)
    {
        Log::logError (
                "Failed to create SDL_Texture from tile data text surface: \""
                        + text + "\" | SDL_ttf Error: " + TTF_GetError ());
        return;
    }
}

void WorldViewport::destroyResources ()
{
    for (uint i = 0; i < 256; ++i)
    {
        SDL_DestroyTexture (m_textTextures[i]);
        m_textTextures[i] = nullptr;
    }
    SDL_DestroyTexture (m_startTexture);
    m_startTexture = nullptr;
    SDL_DestroyTexture (m_endTexture);
    m_endTexture = nullptr;
}

} /* namespace gui */
} /* namespace pathFind */
