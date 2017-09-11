/*
 * WorldViewport.cc
 */

#include "WorldViewport.h"

namespace parPath
{

WorldViewport::WorldViewport (SDL_Rect rect, std::string worldFile,
        SDL_Color backgroundColor)
        : Viewport (rect, backgroundColor),
          m_worldFile (worldFile)
{

}

WorldViewport::~WorldViewport ()
{
}

void WorldViewport::render (SDL_Renderer* renderer)
{

}

void WorldViewport::handleEvent (SDL_Event& e)
{

}

void WorldViewport::loadFile (std::string worldFile)
{

}

} /* namespace parPath */
