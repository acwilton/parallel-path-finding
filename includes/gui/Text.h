/**
 * File        : Text.h
 * Description : Renders text in a viewport
 */

#ifndef TEXT_H_
#define TEXT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <functional>
#include <string>

#include "gui/Error.h"

namespace pathFind
{

namespace gui
{

class Text
{
public:
    Text (int vp_x, int vp_y, std::string text, SDL_Rect rect, size_t fontSize,
          SDL_Color textColor = { 0x00, 0x00, 0x00, 0xFF });
    virtual ~Text ();

    virtual int getX () const;
    virtual int getY () const;
    virtual int getWidth () const;
    virtual int getHeight () const;
    virtual std::string getText () const;
    virtual size_t getFontSize () const;
    virtual SDL_Color getTextColor () const;
    virtual bool isEnabled () const;

    virtual void enable ();
    virtual void disable ();

    virtual void setText (std::string text);
    virtual void setFontSize (size_t fontSize);
    virtual void setTextColor (SDL_Color color);

    virtual void render (SDL_Renderer* renderer);

protected:
    virtual void initializeTextTexture (SDL_Renderer* renderer);
    virtual void destroyResources ();

    int m_viewport_x;
    int m_viewport_y;

    std::string m_text;
    uint m_fontSize;

    SDL_Color m_textColor;

    bool m_textInitialized;
    SDL_Texture* m_textTexture;
    SDL_Rect m_textRect;

    bool m_enabled;

};

} /* namespace gui */
} /* namespace pathFind */

#endif /* TEXT_H_ */
