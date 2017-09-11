/**
 * File        : GraphicTile.h
 * Description : The graphical display of a world tile. Held under WorldViewport
 */

#ifndef GRAPHICTILE_H_
#define GRAPHICTILE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>

#include "Error.h"
#include "World.h"

namespace parPath
{

class GraphicTile
{
public:
    GraphicTile (World::tile_t tileData, SDL_Rect rect);
    virtual ~GraphicTile ();

    void setRect (SDL_Rect rect);
    void setX (int x);
    void setY (int y);
    void setW (int w);
    void setH (int h);

    void setTextEnabled (bool textEnabled);
    bool isTextEnabled () const;

    SDL_Rect getRect () const;
    World::tile_t getTile () const;

    void render (SDL_Renderer* renderer);

protected:
    World::tile_t m_tileData;

    SDL_Rect m_rect;
    SDL_Rect m_textRect;
    SDL_Color m_rectColor;
    SDL_Color m_textColor;
    SDL_Texture* m_textTexture;

    bool m_textInitialized;
    bool m_textEnabled;

    void initializeTexture (SDL_Renderer* renderer);
    void destroyResources ();
};

} /* namespace parPath */

#endif /* GRAPHICTILE_H_ */
