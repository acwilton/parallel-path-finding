/**
 * File        : Window.h
 * Description : Acts as a wrapper class for SDL_Window as well as maintains the components of the window
 *               like the renderer, viewports, etc.
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <string>

#include "Error.h"

#ifndef WINDOW_H_
#define WINDOW_H_

namespace parPath
{

class Window final
{
public:

    Window(std::string title, size_t width, size_t height);
    ~Window();

private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    std::vector<SDL_Rect> m_view_port;

};

} /* namespace parPath */

#endif /* WINDOW_H_ */
