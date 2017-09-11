/**
 * File        : WorldViewport.h
 * Description : 
 */

#ifndef WORLDVIEWPORT_H_
#define WORLDVIEWPORT_H_

#include "Viewport.h"
#include "World.h"

namespace parPath
{

class WorldViewport: public Viewport
{
public:
    WorldViewport (SDL_Rect rect, std::string worldFile,
            SDL_Color backgroundColor = { 0xFF, 0xFF, 0xFF, 0xFF });
    virtual ~WorldViewport ();

    virtual void render (SDL_Renderer* renderer);

    virtual void handleEvent (SDL_Event& e);

    virtual void loadFile (std::string worldFile);

protected:
    std::string m_worldFile;
};

} /* namespace parPath */

#endif /* WORLDVIEWPORT_H_ */
