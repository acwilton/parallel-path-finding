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

#include "gui/Error.h"
#include "gui/Viewport.h"

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
    std::shared_ptr<Viewport> getViewport (uint pos);
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

    /**
     * Start up the window using the settings given to the object thus far.
     */
    void spawnWindow ();
    /**
     * Close the window that is currently up for this object
     */
    void closeWindow ();

private:

    void destroyResources ();

    std::string m_title;
    size_t m_width;
    size_t m_height;

    bool m_keyFocus;
    bool m_mouseFocus;
    bool m_minimized;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    std::vector<std::shared_ptr<Viewport>> m_viewports;
};

} /* namespace parPath */

#endif /* WINDOW_H_ */
