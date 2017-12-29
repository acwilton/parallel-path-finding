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

#include "gui/Error.h"
#include "gui/Button.h"
#include "gui/Text.h"

namespace pathFind
{

namespace gui
{

class Viewport
{
public:
    Viewport (SDL_Rect rect, SDL_Color backgroundColor =
    { 0xFF, 0xFF, 0xFF, 0xFF });
    virtual ~Viewport ();

    virtual int getX () const;
    virtual int getY () const;
    virtual int getWidth () const;
    virtual int getHeight () const;
    virtual bool isEnabled () const;
    virtual SDL_Color getBackgroundColor () const;
    virtual std::shared_ptr<Button> getButton (uint pos);
    virtual std::shared_ptr<Text> getText (uint pos);

    virtual void setBackgroundColor (SDL_Color color);

    virtual void enable ();
    virtual void disable ();

    virtual void addButton (std::shared_ptr<Button> b);
    virtual void removeButton (uint pos);

    virtual void addText (std::shared_ptr<Text> t);
    virtual void removeText (uint pos);

    virtual void render (SDL_Renderer* renderer);

    virtual void handleEvent (SDL_Event& e);

protected:
    std::vector<std::shared_ptr<Button>> m_buttons;
    std::vector<std::shared_ptr<Text>> m_texts;
    SDL_Rect m_rect;

    SDL_Color m_backgroundColor;

    bool m_enabled;
};

} /* namespace gui */
} /* namespace pathFind */

#endif /* VIEWPORT_H_ */
