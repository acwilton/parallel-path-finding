/*
 * TextInput.cc
 */
#include "gui/TextInput.h"

namespace pathFind
{

namespace gui
{

TextInput::TextInput (int x, int y, size_t fontSize,
        std::function<void (std::string)> funct,
        SDL_Color backgroundColor, SDL_Color textColor)
    : Button (0, 0, "", SDL_Rect
            {x, y, static_cast<int>(fontSize), static_cast<int>(fontSize * 2.25f)},
            fontSize, [](){}, backgroundColor, textColor),
      m_stringFunct (funct),
      m_originalX (x),
      m_originalY (y)
{
}

TextInput::~TextInput ()
{
}

void TextInput::enable ()
{
    m_enabled = true;
    SDL_StartTextInput ();
}

void TextInput::disable ()
{
    m_enabled = false;
    SDL_StopTextInput ();
}

void TextInput::execute ()
{
    m_stringFunct (m_text);
}

void TextInput::handleEvent(SDL_Event& e)
{
    if (!isEnabled ())
    {
        return;
    }

    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.sym == SDLK_BACKSPACE && m_text.length () > 0)
        {
            if ((SDL_GetModState () & KMOD_CTRL))
            {
                m_text = "";
            }
            else
            {
                m_text.pop_back ();
            }
            m_textInitialized = false;
        }
        else if (e.key.keysym.sym == SDLK_c && (SDL_GetModState () & KMOD_CTRL))
        {
            SDL_SetClipboardText (m_text.c_str());
        }
        else if (e.key.keysym.sym == SDLK_v && (SDL_GetModState () & KMOD_CTRL))
        {
            m_text = SDL_GetClipboardText ();
            m_textInitialized = false;
        }
        else if (e.key.keysym.sym == SDLK_RETURN)
        {
            disable ();
            execute ();
        }
        else if (e.key.keysym.sym == SDLK_ESCAPE)
        {
            disable ();
        }
    }
    else if (e.type == SDL_TEXTINPUT)
    {
        if (!(SDL_GetModState () & KMOD_CTRL))
        {
            if (m_text == " ")
            {
                m_text = e.text.text;
            }
            else
            {
                m_text += e.text.text;
            }
            m_textInitialized = false;
        }
    }
}

void TextInput::render (SDL_Renderer* renderer)
{
    if (!isEnabled ())
    {
        return;
    }
    if (!m_textInitialized)
    {
        initializeTextTexture (renderer);
        m_textRect.x = m_originalX - (m_textRect.w / 2);

        m_buttonRect.x = m_textRect.x - 5;
        m_buttonRect.w = m_textRect.w + 10;
    }


    Button::render(renderer);
}

} /* namespace gui */
} /* namespace pathFind */
