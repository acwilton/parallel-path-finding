/**
 * File        : Viewport.h
 * Description : Class meant to handle rendering a certian portion of the screen
 *               as well as hold the buttons inside that portion of the screen
 */

#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include <SDL2/SDL.h>

#include <map>
#include <memory>

#include "Error.h"
#include "Button.h"

namespace parPath
{

class Viewport
{
public:
    Viewport ();
    virtual ~Viewport ();

    virtual std::shared_ptr<Button> getMutableButton (uint key);
    virtual void addButton (uint key, std::shared_ptr<Button> b);
    virtual void removeButton (uint key);

protected:
    std::map<uint, std::shared_ptr<Button>> m_buttons;
};

} /* namespace parPath */

#endif /* VIEWPORT_H_ */
