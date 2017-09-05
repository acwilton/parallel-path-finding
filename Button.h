/**
 * File        : Button.h
 * Description : Button class that executes a given function when clicked on
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Error.h"

namespace parPath
{

class Button
{
public:
    Button ();
    virtual ~Button ();
};

} /* namespace parPath */

#endif /* BUTTON_H_ */
