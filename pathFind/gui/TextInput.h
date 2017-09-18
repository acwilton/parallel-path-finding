/**
 * File        : TextInput.h
 * Description : A quick hacky class for getting text input and then executing
 *               a function with that text as a parameter
 */

#ifndef TEXTINPUT_H_
#define TEXTINPUT_H_

#include "Button.h"

namespace parPath
{

class TextInput: public Button
{
public:
    TextInput (int x, int y, size_t fontSize,
            std::function<void (std::string)> funct,
            SDL_Color backgroundColor = {0xFF, 0xFF, 0xFF, 0xFF},
            SDL_Color textColor = {0x00, 0x00, 0x00, 0xFF});
    virtual ~TextInput ();

    virtual void enable ();
    virtual void disable ();
    virtual void execute ();
    virtual void handleEvent (SDL_Event& e);

    virtual void render (SDL_Renderer* renderer);

protected:
    std::function<void (std::string)> m_stringFunct;
    int m_originalX;
    int m_originalY;
};

} /* namespace parPath */

#endif /* TEXTINPUT_H_ */
