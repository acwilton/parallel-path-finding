/**
 * File        : Button.h
 * Description : Button class that executes a given function when clicked on
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <functional>
#include <string>

#include "Error.h"

namespace parPath
{

class Button
{
public:
    Button (std::string text, SDL_Rect rect, std::function<void ()> funct,
            SDL_Color backgroundColor =
            { 0x77, 0x77, 0x77, 0xFF }, SDL_Color textColor =
            { 0x00, 0x00, 0x00, 0xFF });
    virtual ~Button ();

    virtual size_t getX () const;
    virtual size_t getY () const;
    virtual size_t getWidth () const;
    virtual size_t getHeight () const;
    virtual std::string getText () const;
    virtual SDL_Color getBackgroundColor () const;
    virtual SDL_Color getTextColor () const;

    virtual void setText (std::string text);
    virtual void setBackgroundColor (SDL_Color color);
    virtual void setTextColor (SDL_Color color);

    virtual void render (SDL_Renderer* renderer);
    virtual void execute ();

protected:
    virtual void initializeTextTexture (SDL_Renderer* renderer);
    virtual void destroyResources ();

    std::string m_text;
    SDL_Rect m_buttonRect;

    std::function<void ()> m_funct;

    SDL_Color m_backgroundColor;
    SDL_Color m_textColor;

    SDL_Texture* m_textTexture;
    SDL_Rect m_textRect;
    bool m_textInitialized;
};

} /* namespace parPath */

#endif /* BUTTON_H_ */
