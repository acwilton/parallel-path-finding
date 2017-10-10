/**
 * File        : GraphicTile.h
 * Description : The graphical display of a world tile. Held under WorldViewport
 */

#ifndef GRAPHICTILE_H_
#define GRAPHICTILE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>

#include "gui/Error.h"
#include "common/World.h"

namespace pathFind
{

namespace gui
{

const SDL_Color DEFAULT_COLOR = {0xFF, 0xFF, 0xFF, 0xFF};

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

    void setRectColor (SDL_Color color);

    void render (SDL_Renderer* renderer, SDL_Texture* texture);

protected:
    World::tile_t m_tileData;

    SDL_Rect m_rect;
    SDL_Rect m_textRect;
    SDL_Color m_rectColor;
    SDL_Color m_textColor;

    bool m_textEnabled;

    void updateTextRect ();
};

} /* namespace gui */
} /* namespace pathFind */

#endif /* GRAPHICTILE_H_ */
