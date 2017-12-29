/**
 * File        : WorldViewport.h
 * Description :
 */

#ifndef WORLDVIEWPORT_H_
#define WORLDVIEWPORT_H_

#include <vector>
#include <string>

#include "common/World.h"
#include "common/Point.h"
#include "common/Results.h"
#include "gui/Error.h"
#include "gui/Viewport.h"
#include "gui/GraphicTile.h"

namespace pathFind
{

namespace gui
{

class WorldViewport: public Viewport
{
public:
    WorldViewport (SDL_Rect rect, SDL_Color backgroundColor =
        { 0xFF, 0xFF, 0xFF, 0xFF });
    virtual ~WorldViewport ();

    virtual void render (SDL_Renderer* renderer);

    virtual void handleEvent (SDL_Event& e);

    virtual void setWorld (const std::string& worldFileName);
    virtual void loadWorld ();

    virtual void runAndLoadPathFinding (const std::string& algorithm);
    virtual void runAndLoadPathFinding (const std::string& algorithm,
                                        uint startX, uint startY, uint endX, uint endY);
    virtual void loadResults (const std::string& algName, const Point& start, const Point& end);
    virtual void setResultsEnabled (bool resultsEnabled);

    virtual std::string getCurrentAlgorithm () const;
    virtual uint getCurrentThreadNum () const;

    virtual uint getCameraX () const;
    virtual uint getCameraY () const;
    virtual uint getTileScale () const;

    virtual void setCameraX (int x);
    virtual void setCameraY (int y);
    virtual void setTileScale (int scale);

    virtual void setTextEnabled (bool textEnabled);

    virtual void setShowEndPoints (bool showEndPoints);

protected:
    enum Mode {VIEW, SELECT};
    enum ViewMode {COST, STAT};
    enum StatMode {INDIVIDUAL, COLLECTIVE};

    Mode m_mode;
    ViewMode m_viewMode;
    StatMode m_statMode;

    std::string m_worldName;
    World m_world;
    std::vector<GraphicTile> m_gTiles;

    uint m_cameraX;
    uint m_cameraY;
    uint m_tileScale; // Acts as a zoom

    bool m_textEnabled;
    bool m_textInitialized;

    std::string m_currentAlgorithm;

    std::vector<std::unordered_map<uint, StatPoint>> m_stats;
    uint m_currentThread;
    uint m_maxProcessCount;

    std::vector<Point> m_results;
    bool m_resultsEnabled;

    bool m_showEndPoints;
    Point m_start;
    Point m_end;

    SDL_Color m_startPrevColor;
    SDL_Color m_endPrevColor;

    SDL_Texture* m_textTextures[256];
    std::vector<SDL_Texture*> m_statTextures;
    SDL_Texture* m_startTexture;
    SDL_Texture* m_endTexture;

    virtual void setMode (Mode mode);

    virtual Point trySelectTile (int mouseX, int mouseY);

    virtual SDL_Color getAlgorithmColor () const;
    virtual SDL_Color getTileColor (uint vpX, uint vpY) const;

    uint getIndex (uint x, uint y) const;
    bool isNull (const Point& p) const;

    virtual uint getCameraWidth () const;
    virtual uint getCameraHeight () const;

    virtual uint getCameraOppX () const;
    virtual uint getCameraOppY () const;

    virtual bool isPointInCameraView (const Point& p) const;

    virtual void resetEndPoints ();

    // Always update scale before position
    virtual void updateGraphicTilesScaleAndPos ();
    virtual void updateGraphicTilesPos ();
    virtual void updateTileColors ();

    virtual void initializeTextures (SDL_Renderer* renderer);
    virtual void initializeTexture (SDL_Renderer* renderer, SDL_Texture*& texture,
                                    const std::string& text, const SDL_Color& color);
    virtual void destroyResources ();
};

} /* namespace gui */
} /* namespace pathFind */

#endif /* WORLDVIEWPORT_H_ */
