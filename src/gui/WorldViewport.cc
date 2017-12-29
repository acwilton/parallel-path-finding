/*
 * WorldViewport.cc
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>

#include "gui/WorldViewport.h"
#include "common/Results.h"

namespace pathFind
{

namespace gui
{

const uint MIN_TILE_SCALE = 2;
const uint MAX_TILE_SCALE = 75;
const uint DEFAULT_TILE_SCALE = 10;

const SDL_Color SELECT_COLOR = {0xFF, 0x77, 0x00, 0xFF};
const SDL_Color DIJKSTRA_COLOR = {0xFF, 0xF4, 0x7F, 0xFF};
const SDL_Color ASTAR_COLOR = {0x82, 0xFF, 0x86, 0xFF};
const SDL_Color BIDIR_COLOR = {0xCB, 0x6B, 0xFF, 0xFF};
const SDL_Color PAR_BIDIR_COLOR = {0xFF, 0x75, 0xF1, 0xFF};
const SDL_Color FRINGE_COLOR = {0x84, 0xB9, 0xFF, 0xFF};
const SDL_Color PAR_FRINGE_COLOR = {0xFF, 0x4B, 0x3A, 0xFF};
const SDL_Color STAT_TEXT_COLOR = {0x00, 0x00, 0x00, 0xFF};
const SDL_Color STAT_TILE_COLOR = {0x2A, 0x2E, 0xFC, 0xFF};

WorldViewport::WorldViewport (SDL_Rect rect, SDL_Color backgroundColor)
        : Viewport (rect, backgroundColor),
          m_mode (VIEW),
          m_viewMode (COST),
          m_statMode (INDIVIDUAL),
          m_worldName (" "),
          m_cameraX (0),
          m_cameraY (0),
          m_tileScale (DEFAULT_TILE_SCALE),
          m_textEnabled (false),
          m_textInitialized (false),
          m_currentAlgorithm ("dijkstra"),
          m_currentThread (0),
          m_maxProcessCount (0),
          m_resultsEnabled (false),
          m_showEndPoints (false),
          m_start (0, 0),
          m_end (0, 0),
          m_startTexture (nullptr),
          m_endTexture (nullptr)
{
    m_gTiles.resize ((getWidth () * getHeight ()) / (MIN_TILE_SCALE * MIN_TILE_SCALE));

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
        for (uint y = 0; y < getCameraHeight (); ++y)
        {
            for (uint x = 0; x < getCameraWidth (); ++x)
            {
                GraphicTile& t = m_gTiles[getIndex (x, y)];

                SDL_Texture* renderTexture = m_textTextures[t.getTile().cost];
                if (m_viewMode == ViewMode::STAT)
                {
                    if (m_statMode == StatMode::INDIVIDUAL)
                    {
                        if (m_currentThread < m_stats.size ())
                        {
                            auto statIter = m_stats[m_currentThread].find (m_world (x + m_cameraX, y + m_cameraY).id);
                            if (statIter == m_stats[m_currentThread].end ())
                            {
                                renderTexture = nullptr;
                            }
                            else
                            {
                                if (m_statTextures[statIter->second.processCount] == nullptr)
                                {
                                    initializeTexture(renderer, m_statTextures[statIter->second.processCount],
                                        std::to_string (statIter->second.processCount), STAT_TEXT_COLOR);
                                }
                                renderTexture = m_statTextures[statIter->second.processCount];
                            }
                        }
                    }
                    else
                    {
                        uint count = 0;
                        for (auto& s : m_stats)
                        {
                            auto statIter = s.find (m_world (x + m_cameraX, y + m_cameraY).id);
                            if (statIter != s.end ())
                            {
                                count += statIter->second.processCount;
                            }
                        }

                        if (count == 0)
                        {
                            renderTexture = nullptr;
                        }
                        else
                        {
                            if (m_statTextures[count] == nullptr)
                            {
                                initializeTexture(renderer, m_statTextures[count],
                                    std::to_string (count), STAT_TEXT_COLOR);
                            }
                            renderTexture = m_statTextures[count];
                        }
                    }
                }
                if (m_showEndPoints)
                {
                    if (m_start.x == x + m_cameraX && m_start.y == y + m_cameraY)
                    {
                        renderTexture = m_startTexture;
                    }
                    else if (m_end.x == x + m_cameraX && m_end.y == y + m_cameraY)
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
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            m_rect.w = e.window.data1;
            m_rect.h = e.window.data2;
        }
        if (e.type == SDL_KEYDOWN)
        {
            uint moveSpeed = 1;
            uint scaleSpeed = 1;
            if ((SDL_GetModState () & KMOD_CTRL))
            {
                moveSpeed = 10;
                scaleSpeed = 2;
            }
            if ((SDL_GetModState () & KMOD_ALT))
            {
                moveSpeed = 30;
                scaleSpeed = 5;
            }
            switch (e.key.keysym.sym)
            {
            case SDLK_UP:
                if (getCameraY () >= moveSpeed)
                {
                    setCameraY (getCameraY () - moveSpeed);
                }
                else
                {
                    setCameraY (0);
                }
                break;
            case SDLK_DOWN:
                if (getCameraOppY () + moveSpeed <= m_world.getHeight ())
                {
                    setCameraY (getCameraY () + moveSpeed);
                }
                else
                {
                    setCameraY (m_cameraY + (m_world.getHeight () - getCameraOppY()));
                }
                break;
            case SDLK_LEFT:
                if (getCameraX () >= moveSpeed)
                {
                    setCameraX (getCameraX () - moveSpeed);
                }
                else
                {
                    setCameraX (0);
                }
                break;
            case SDLK_RIGHT:
                if (getCameraOppX () + moveSpeed <= m_world.getWidth ())
                {
                    setCameraX (getCameraX () + moveSpeed);
                }
                else
                {
                    setCameraX (m_cameraX + (m_world.getWidth () - getCameraOppX()));
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
                // If below zero it wraps around to uint::max so just check that it doesn't do that either
                if (m_tileScale - scaleSpeed > MIN_TILE_SCALE && m_tileScale - scaleSpeed < MAX_TILE_SCALE)
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
            case SDLK_LEFTBRACKET:
                if (m_currentThread != 0)
                {
                    --m_currentThread;
                }
                else
                {
                    m_currentThread = m_stats.size () - 1;
                }
                updateGraphicTilesPos ();
                break;
            case SDLK_RIGHTBRACKET:
                if (m_currentThread + 1 < m_stats.size ())
                {
                    ++m_currentThread;
                }
                else
                {
                    m_currentThread = 0;
                }
                updateGraphicTilesPos ();
                break;
            case SDLK_v:
                if (m_viewMode == COST)
                {
                    m_viewMode = STAT;
                }
                else
                {
                    m_viewMode = COST;
                }
                updateGraphicTilesPos();
                break;
            case SDLK_s:
                if (m_statMode == INDIVIDUAL)
                {
                    m_statMode = COLLECTIVE;
                }
                else
                {
                    m_statMode = INDIVIDUAL;
                }
                updateGraphicTilesPos();
                break;
            case SDLK_r:
                if (!isNull (m_start) && !isNull (m_end))
                {
                    runAndLoadPathFinding (m_currentAlgorithm);
                }
                break;
            case SDLK_d:
                if (!isNull (m_start) && !isNull (m_end))
                {
                    loadResults ("dijkstra", m_start, m_end);
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
                break;
            case SDLK_a:
                if (!isNull (m_start) && !isNull (m_end))
                {
                    loadResults ("aStar", m_start, m_end);
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
                break;
            case SDLK_b:
                if (!isNull (m_start) && !isNull (m_end))
                {
                    loadResults ("bidir", m_start, m_end);
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
                break;
            case SDLK_p:
                if (!isNull (m_start) && !isNull (m_end))
                {
                    loadResults ("parBidir", m_start, m_end);
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
                break;
            case SDLK_f:
                if (!isNull (m_start) && !isNull (m_end))
                {
                    loadResults ("fringe", m_start, m_end);
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
                break;
            case SDLK_g:
                if (!isNull (m_start) && !isNull (m_end))
                {
                    loadResults ("parFringe", m_start, m_end);
                }
                else
                {
                    setResultsEnabled (!m_resultsEnabled);
                }
                break;
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
                        GraphicTile& t = m_gTiles[getIndex (m_start.x - m_cameraX, m_start.y - m_cameraY)];
                        m_startPrevColor = t.getRectColor ();
                        t.setRectColor (SELECT_COLOR);

                        // Reset m_end
                        m_end.x = m_world.getWidth ();
                        m_end.y = m_world.getHeight ();
                    }
                }
                else if (m_mode == SELECT)
                {
                    if (isNull (m_end))
                    {
                        m_end = trySelectTile (mouseX, mouseY);
                        if (!isNull (m_end))
                        {
                            GraphicTile& t = m_gTiles[getIndex (m_end.x - m_cameraX, m_end.y - m_cameraY)];
                            m_endPrevColor = t.getRectColor();
                            t.setRectColor(SELECT_COLOR);
                        }
                    }
                    else
                    {
                        setMode (VIEW);
                        m_gTiles[getIndex (m_start.x - m_cameraX, m_start.y - m_cameraY)].setRectColor(m_startPrevColor);
                        m_gTiles[getIndex (m_end.x - m_cameraX, m_end.y - m_cameraY)].setRectColor(m_endPrevColor);
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
    m_cameraX = 0;
    m_cameraY = 0;

    std::string worldFileName = "../worlds/" + m_worldName + ".world";
    Log::logInfo("Created world: " + worldFileName);
    std::ifstream worldFile(worldFileName, std::ifstream::in | std::ifstream::binary);
    if (!worldFile)
    {
        Log::logError ("WorldViewport either could not find or could not open "
                + worldFileName);
        return;
    }

    worldFile >> m_world;

    resetEndPoints ();

    updateGraphicTilesScaleAndPos ();
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
    m_threadColors.clear ();
    loadResults (m_currentAlgorithm, m_start, m_end);
}

void WorldViewport::runAndLoadPathFinding (const std::string& algorithm,
                                    uint startX, uint startY, uint endX, uint endY)
{
    m_start.x = startX;
    m_start.y = startY;
    m_end.x = endX;
    m_end.y = endY;
    runAndLoadPathFinding (algorithm);
}

void WorldViewport::loadResults (const std::string& algName, const Point& start, const Point& end)
{
    m_currentAlgorithm = algName;
    setResultsEnabled (false);
    m_results.clear();
    m_stats.clear();
    if (!readResults (m_results, m_stats, m_maxProcessCount, start, end, m_worldName, algName))
    {
        runAndLoadPathFinding (algName, start.x, start.y, end.x, end.y);
        return;
    }
    if (m_threadColors.size () != m_stats.size () + 1)
    {
        m_threadColors = getRandomColors(m_stats.size () + 1);
    }
    if (m_maxProcessCount * m_stats.size () > m_statTextures.size ())
    {
        m_statTextures.resize (m_maxProcessCount * m_stats.size (), nullptr);
    }
    if (m_mode == VIEW)
    {
        if (!m_results.empty())
        {
            m_start = m_results.front();
            m_end = m_results.back();
        }
    }
    setResultsEnabled (true);
}

void WorldViewport::setResultsEnabled (bool resultsEnabled)
{
    m_resultsEnabled = resultsEnabled;
    updateTileColors ();
    SDL_Color algColor = getAlgorithmColor ();
    for (auto& r : m_results)
    {
        if (isPointInCameraView (r))
        {
            uint vpX = r.x - m_cameraX;
            uint vpY = r.y - m_cameraY;
            SDL_Color color = m_resultsEnabled ? algColor : getTileColor(vpX, vpY);
            m_gTiles[getIndex (vpX, vpY)].setRectColor(color);
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
    updateGraphicTilesPos ();
}

void WorldViewport::setCameraY (int y)
{
    if (y < 0)
    {
        y = 0;
    }
    m_cameraY = y;
    updateGraphicTilesPos ();
}

void WorldViewport::setTileScale (int scale)
{
    m_tileScale = scale;

    size_t absHeight = static_cast<size_t> (m_rect.h) / m_tileScale;
    if (getCameraY () + absHeight > m_world.getHeight ())
    {
        m_cameraY = absHeight > m_world.getHeight () ? 0 : m_world.getWidth () - absHeight;
    }
    size_t absWidth = static_cast<size_t> (m_rect.w) / m_tileScale;
    if (getCameraX() + absWidth > m_world.getWidth ())
    {
        m_cameraX = absWidth > m_world.getWidth () ? 0 : m_world.getWidth () - absWidth;
    }

    updateGraphicTilesScaleAndPos ();
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

std::string WorldViewport::getCurrentAlgorithm() const
{
    return m_currentAlgorithm;
}

uint WorldViewport::getCurrentThreadNum () const
{
    return m_currentThread;
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
    Point tile (m_world.getWidth (), m_world.getHeight ());

    uint tileX = m_cameraX + (static_cast<uint> (mouseX) / m_tileScale);
    uint tileY = m_cameraY + (static_cast<uint> (mouseY) / m_tileScale);
    if (tileX < m_world.getWidth () && tileY < m_world.getHeight ())
    {
        if (m_world(tileX, tileY).cost != 0)
        {
            tile.x = tileX;
            tile.y = tileY;
        }
    }
    return tile;
}

SDL_Color WorldViewport::getAlgorithmColor () const
{
    if (m_viewMode == STAT)
    {
        return STAT_TILE_COLOR;
    }
    if (m_currentAlgorithm == "dijkstra")
    {
        return DIJKSTRA_COLOR;
    }
    else if (m_currentAlgorithm == "aStar")
    {
        return ASTAR_COLOR;
    }
    else if (m_currentAlgorithm == "bidir")
    {
        return BIDIR_COLOR;
    }
    else if (m_currentAlgorithm == "parBidir")
    {
        return PAR_BIDIR_COLOR;
    }
    else if (m_currentAlgorithm == "fringe")
    {
        return FRINGE_COLOR;
    }
    else if (m_currentAlgorithm == "parFringe")
    {
        return PAR_FRINGE_COLOR;
    }

    return DEFAULT_COLOR;
}

SDL_Color WorldViewport::getTileColor (uint vpX, uint vpY) const
{
    if (m_viewMode == COST)
    {
        return DEFAULT_COLOR;
    }
    else
    {
        if (m_currentThread < m_stats.size ())
        {
            auto statIter = m_stats[m_currentThread].find (m_world (vpX + m_cameraX, vpY + m_cameraY).id);
            if (statIter == m_stats[m_currentThread].end ())
            {
                return DEFAULT_COLOR;
            }
            else
            {
                float processFactor = 0.0f;
                if (m_maxProcessCount > 1)
                {
                    processFactor = static_cast<float> (statIter->second.processCount - 1) / (m_maxProcessCount - 1);
                }
                return processFactor > 0.5f ?
                      SDL_Color {255, static_cast<unsigned char> ((255 - 42) * (2.0f - processFactor * 2) + 42), 42, 255}
                    : SDL_Color {static_cast<unsigned char> ((255 - 42) * processFactor * 2 + 42), 255, 42, 255};
            }
        }
    }
    return DEFAULT_COLOR;
}

std::vector<SDL_Color> WorldViewport::getRandomColors (uint numRandomColors) const
{
    std::mt19937 generator (0);
    std::uniform_int_distribution<int> distribution (42, 200);

    std::vector<SDL_Color> colors (numRandomColors);
    for (auto& c : colors)
    {
        c.a = 255;
        c.r = distribution (generator);
        c.g = distribution (generator);
        c.b = distribution (generator);
    }

    return colors;
}

uint WorldViewport::getIndex (uint x, uint y) const
{
    return (ceil (m_rect.w / MIN_TILE_SCALE) * y) + x;
}

bool WorldViewport::isNull(const Point& p) const
{
    return (p.x == m_world.getWidth () && p.y == m_world.getHeight ());
}

uint WorldViewport::getCameraWidth () const
{
    return std::min
    (
        static_cast<size_t> (ceil (static_cast<float> (m_rect.w) / m_tileScale)),
        m_world.getWidth () - m_cameraX
    );
}

uint WorldViewport::getCameraHeight () const
{
    return std::min
    (
        static_cast<size_t> (ceil (static_cast<float> (m_rect.h) / m_tileScale)),
        m_world.getHeight () - m_cameraY
    );
}

uint WorldViewport::getCameraOppX () const
{
    return m_cameraX + std::min
    (
        static_cast<size_t> (m_rect.w) / m_tileScale,
        m_world.getWidth () - m_cameraX
    );
}

uint WorldViewport::getCameraOppY () const
{
    return m_cameraY + std::min
    (
        static_cast<size_t> (m_rect.h) / m_tileScale,
        m_world.getHeight () - m_cameraY
    );
}

bool WorldViewport::isPointInCameraView (const Point& p) const
{
    return (p.x >= m_cameraX && p.y >= m_cameraY &&
            p.x < getCameraOppX () && p.y < getCameraOppY ());
}

void WorldViewport::resetEndPoints()
{
    // a start/end 1 past the last tile's x and y is considered a null value
    m_start.x = m_end.x = m_world.getWidth ();
    m_start.y = m_end.y = m_world.getHeight ();
}

void WorldViewport::updateGraphicTilesScaleAndPos ()
{
    for (uint y = 0; y < getCameraHeight (); ++y)
    {
        for (uint x = 0; x < getCameraWidth (); ++x)
        {
            GraphicTile& t = m_gTiles[getIndex (x, y)];
            t.setTile(m_world (x + m_cameraX, y + m_cameraY));
            t.setW (m_tileScale);
            t.setH (m_tileScale);
            t.setX(x * m_tileScale);
            t.setY(y * m_tileScale);
        }
    }

    updateTileColors ();

    if (m_showEndPoints && m_mode == SELECT)
    {
        if (isPointInCameraView (m_start))
        {
            m_gTiles[getIndex (m_start.x - m_cameraX, m_start.y - m_cameraY)].setRectColor(SELECT_COLOR);
        }
        if (isPointInCameraView (m_end))
        {
            m_gTiles[getIndex (m_end.x - m_cameraX, m_end.y - m_cameraY)].setRectColor(SELECT_COLOR);
        }
    }

    if (m_resultsEnabled)
    {
        for (auto& r : m_results)
        {
            if (isPointInCameraView (r))
            {
                m_gTiles[getIndex (r.x - m_cameraX, r.y - m_cameraY)].setRectColor(getAlgorithmColor ());
            }
        }
    }
}

void WorldViewport::updateGraphicTilesPos ()
{
    for (uint y = 0; y < getCameraHeight (); ++y)
    {
        for (uint x = 0; x < getCameraWidth (); ++x)
        {
            m_gTiles[getIndex (x, y)].setTile (m_world (x + m_cameraX, y + m_cameraY));
        }
    }

    updateTileColors ();

    if (m_showEndPoints && m_mode == SELECT)
    {
        if (isPointInCameraView (m_start))
        {
            m_gTiles[getIndex (m_start.x - m_cameraX, m_start.y - m_cameraY)].setRectColor(SELECT_COLOR);
        }
        if (isPointInCameraView (m_end))
        {
            m_gTiles[getIndex (m_end.x - m_cameraX, m_end.y - m_cameraY)].setRectColor(SELECT_COLOR);
        }
    }

    if (m_resultsEnabled)
    {
        for (auto& r : m_results)
        {
            if (isPointInCameraView (r))
            {
                m_gTiles[getIndex (r.x - m_cameraX, r.y - m_cameraY)].setRectColor(getAlgorithmColor ());
            }
        }
    }
}

void WorldViewport::updateTileColors ()
{
    if (m_viewMode == COST)
    {
        for (auto& gt : m_gTiles)
        {
            if (gt.getTile ().cost != 0)
            {
                gt.setRectColor(DEFAULT_COLOR);
            }
        }
    }
    else
    {
        if (m_statMode == INDIVIDUAL)
        {
            for (uint y = 0; y < getCameraHeight (); ++y)
            {
                for (uint x = 0; x < getCameraWidth (); ++x)
                {
                    GraphicTile& t = m_gTiles[getIndex (x, y)];
                    if (t.getTile().cost != 0)
                    {
                        t.setRectColor(getTileColor (x, y));
                    }
                }
            }
        }
        else
        {
            for (uint y = 0; y < getCameraHeight (); ++y)
            {
                for (uint x = 0; x < getCameraWidth (); ++x)
                {
                    GraphicTile& t = m_gTiles[getIndex (x, y)];
                    if (t.getTile().cost != 0)
                    {
                        bool found = false;
                        bool multipleFound = false;
                        uint threadId = 0;
                        for (uint i = 0; i < m_stats.size (); ++i)
                        {
                            auto statIter = m_stats[i].find (m_world (x + m_cameraX, y + m_cameraY).id);
                            if (statIter != m_stats[i].end ())
                            {
                                if (found)
                                {
                                    multipleFound = true;
                                    break;
                                }
                                found = true;
                                threadId = i;
                            }
                        }
                        if (multipleFound)
                        {
                            t.setRectColor(m_threadColors[m_stats.size ()]);
                        }
                        else if (found)
                        {
                            //std::cout << "i: " << threadId << "\n";
                            t.setRectColor (m_threadColors[threadId]);
                        }
                        else
                        {
                            t.setRectColor (DEFAULT_COLOR);
                        }
                    }
                }
            }
        }
    }
}

void WorldViewport::initializeTextures (SDL_Renderer* renderer)
{
    destroyResources ();
    for (uint i = 0; i < 256; ++i)
    {
        initializeTexture (renderer, m_textTextures[i], std::to_string (i), SDL_Color {0x00, 0x00, 0x00, 0xFF});
    }
    initializeTexture (renderer, m_startTexture, "S", SDL_Color {0x00, 0x00, 0x00, 0xFF});
    initializeTexture (renderer, m_endTexture, "E", SDL_Color {0x00, 0x00, 0x00, 0xFF});

    m_textInitialized = true;

    return;
}

void WorldViewport::initializeTexture(SDL_Renderer* renderer, SDL_Texture*& texture,
                                      const std::string& text, const SDL_Color& color)
{
    TTF_Font* font = TTF_OpenFont ("../resources/FreeSans.ttf", 128);
    SDL_Surface* textSurface = TTF_RenderText_Solid (font, text.c_str(),
                                                     color);
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
