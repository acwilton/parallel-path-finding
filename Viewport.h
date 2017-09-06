/**
 * File        : Viewport.h
 * Description : Class meant to handle rendering a certian portion of the screen
 *               as well as hold the buttons inside that portion of the screen
 */

#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include <SDL2/SDL.h>

#include <vector>
#include <memory>

#include "Error.h"
#include "Button.h"

namespace parPath
{

class Viewport
{
public:
    Viewport (SDL_Rect rect, SDL_Color backgroundColor =
    { 0xFF, 0xFF, 0xFF, 0xFF });
    virtual ~Viewport ();

    virtual size_t getX () const;
    virtual size_t getY () const;
    virtual size_t getWidth () const;
    virtual size_t getHeight () const;
    virtual SDL_Color getBackgroundColor () const;
    virtual std::shared_ptr<Button> getButton (uint pos) const;

    virtual void setBackgroundColor (SDL_Color color);

    virtual void addButton (std::shared_ptr<Button> b);
    virtual void removeButton (uint pos);

    virtual void render (SDL_Renderer* renderer);

    virtual void handleEvent (SDL_Event* e);

protected:
    std::vector<std::shared_ptr<Button>> m_buttons;
    SDL_Rect m_rect;

    SDL_Color m_backgroundColor;
};

} /* namespace parPath */

#endif /* VIEWPORT_H_ */
