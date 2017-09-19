/**
 * File        : PathTile.h
 * Description : A representation of a tile in the world meant for path finding.
 *               holds things like the cost to enter the edge, the current total
 *               cost to get here from the source, etc.
 */

#ifndef PATHTILE_H_
#define PATHTILE_H_

#include "common/World.h"

namespace parPath
{

class PathTile
{
public:
    PathTile ();
    virtual ~PathTile ();
};

} /* namespace parPath */

#endif /* PATHTILE_H_ */
