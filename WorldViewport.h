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
    WorldViewport ();
    virtual ~WorldViewport ();
};

} /* namespace parPath */

#endif /* WORLDVIEWPORT_H_ */
