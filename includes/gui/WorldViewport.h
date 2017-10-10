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

protected:
    uint getIndex (uint x, uint y) const;

    std::string m_worldName;
    std::vector<GraphicTile> m_gTiles;
    size_t m_worldWidth;
    size_t m_worldHeight;

    uint m_cameraX;
    uint m_cameraY;
    uint m_tileScale; // Acts as a zoom

    bool m_textEnabled;
    bool m_textInitialized;

    std::vector<Point> m_results;
    bool m_resultsEnabled;

    SDL_Texture* m_textTextures[256];

    uint getCameraOppX () const;
    uint getCameraOppY () const;

    void initializeTextures (SDL_Renderer* renderer);
    void destroyResources ();
};

} /* namespace gui */
} /* namespace pathFind */

#endif /* WORLDVIEWPORT_H_ */
