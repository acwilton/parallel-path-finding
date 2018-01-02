/**
 * File        : ToolbarViewport.h
 * Description : A specialization of a viewport that interacts with a worldviewport
 */

 #ifndef TOOLBARVIEWPORT_H_
 #define TOOLBARVIEWPORT_H_

#include "Viewport.h"
#include "WorldViewport.h"

namespace pathFind
{

namespace gui
{

class ToolbarViewport : public Viewport
{
public:
    ToolbarViewport (SDL_Rect rect, std::shared_ptr<WorldViewport> wvp,
        uint windowHeight, SDL_Color backgroundColor = { 0xFF, 0xFF, 0xFF, 0xFF });
    virtual ~ToolbarViewport ();

    virtual void render (SDL_Renderer* renderer);
    virtual void handleEvent (SDL_Event& e);
protected:
    std::shared_ptr<WorldViewport> m_wvp;

    Text m_currentAlg;
    Text m_currentThreadNum;

    uint m_yFromBottom;
};

} /* gui */
} /* pathFind */

#endif /* TOOLBARVIEWPORT_H_ */
