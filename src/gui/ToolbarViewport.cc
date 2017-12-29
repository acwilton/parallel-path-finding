
#include "gui/ToolbarViewport.h"
#include "gui/Constants.h"

namespace pathFind
{

namespace gui
{

ToolbarViewport::ToolbarViewport (SDL_Rect rect, std::shared_ptr<WorldViewport> wvp,
    SDL_Color backgroundColor)
    : Viewport (rect, backgroundColor),
      m_wvp (wvp),
      m_currentAlg (rect.x, rect.y, "dijkstra",
        SDL_Rect {rect.w - 300, rect.h / 2 - 25, 280, 50}, 16)
{
    m_currentAlg.enable();
}

ToolbarViewport::~ToolbarViewport ()
{
}

void ToolbarViewport::render (SDL_Renderer* renderer)
{
    std::string currentAlg = m_wvp->getCurrentAlgorithm ();
    if (currentAlg != m_currentAlg.getText ())
    {
        m_currentAlg.setText (currentAlg);
    }
    Viewport::render (renderer);
    if (m_enabled)
    {
        m_currentAlg.render(renderer);
    }
}

} /* gui */
} /* pathFind */
