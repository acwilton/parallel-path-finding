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
    virtual void loadResults (const Point& start, const Point& end,
                             const std::string& algName);
    virtual void setResultsEnabled (bool resultsEnabled);

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

    Mode m_mode;

    std::string m_worldName;
    std::vector<GraphicTile> m_gTiles;
    size_t m_worldWidth;
    size_t m_worldHeight;

    uint m_cameraX;
    uint m_cameraY;
    uint m_tileScale; // Acts as a zoom

    bool m_textEnabled;
    bool m_textInitialized;

    std::string m_currentAlgorithm;
    std::vector<Point> m_results;
    bool m_resultsEnabled;

    bool m_showEndPoints;
    Point m_start;
    Point m_end;

    SDL_Color m_startPrevColor;
    SDL_Color m_endPrevColor;

    SDL_Texture* m_textTextures[256];
    SDL_Texture* m_startTexture;
    SDL_Texture* m_endTexture;

    virtual void setMode (Mode mode);

    virtual Point trySelectTile (int mouseX, int mouseY);

    virtual SDL_Color getAlgorithmColor () const;

    uint getIndex (uint x, uint y) const;
    bool isNull (const Point& p) const;

    virtual uint getCameraOppX () const;
    virtual uint getCameraOppY () const;

    virtual void resetEndPoints ();

    virtual void initializeTextures (SDL_Renderer* renderer);
    virtual void initializeTexture (SDL_Renderer* renderer, SDL_Texture*& texture,
                                    const std::string& text);
    virtual void destroyResources ();
};

} /* namespace gui */
} /* namespace pathFind */

#endif /* WORLDVIEWPORT_H_ */
