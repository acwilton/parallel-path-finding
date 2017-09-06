/**
 * File        : Window.h
 * Description : Acts as a wrapper class for SDL_Window as well as maintains the components of the window
 *               like the renderer, viewports, etc.
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL2/SDL.h>

#include <vector>
#include <string>
#include <memory>

#include "Error.h"
#include "Viewport.h"

namespace parPath
{

class Window final
{
public:

    Window (std::string title, size_t width, size_t height);
    ~Window ();

    size_t getWidth () const;
    size_t getHeight () const;
    std::string getTitle () const;
    std::shared_ptr<Viewport> getViewport (uint pos) const;
    bool hasMouseFocus () const;
    bool hasKeyFocus () const;
    bool isMinimized () const;
    bool isOpen () const;

    void setWidth (size_t width);
    void setHeight (size_t height);
    void setTitle (std::string title);

    /**
     * Add a viewport object into our window for rendering/interaction
     */
    void addViewport (std::shared_ptr<Viewport> vp);
    /**
     * Remove a viewport object from our window
     */
    void removeViewport (uint pos);

    void focus ();

    void handleEvent (SDL_Event& e);

    void render ();

    void spawnWindow ();
    void closeWindow ();

private:

    void destroyResources ();

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    std::vector<std::shared_ptr<Viewport>> m_viewports;

    size_t m_width;
    size_t m_height;
    std::string m_title;

    uint m_windowID;

    bool m_keyFocus;
    bool m_mouseFocus;
    bool m_minimized;
};

} /* namespace parPath */

#endif /* WINDOW_H_ */
