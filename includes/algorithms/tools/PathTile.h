/**
 * File        : PathTile.h
 * Description : A representation of a tile in the world meant for path finding.
 *               holds things like the cost to enter the edge, the current total
 *               cost to get here from the source, etc.
 */

#ifndef PATHTILE_H_
#define PATHTILE_H_

#include <memory>
#include <limits>

#include "common/World.h"

typedef unsigned int uint;

namespace pathFind
{

class PathTile
{
public:

    const uint INF = std::numeric_limits<uint>::max();

    PathTile ();
    PathTile (World::tile_t tile, uint x, uint y);
    PathTile (const PathTile& other);
    virtual ~PathTile ();

    PathTile& operator= (const PathTile& rhs);

    void setTile (World::tile_t tile);
    World::tile_t getTile () const;

    void setBestCost (uint bestCost);
    uint getBestCost () const;

    uint x () const;
    uint y () const;

    uint bestX () const;
    uint bestY () const;
    void setBestTile (uint x, uint y);

    bool operator< (const PathTile& rhs) const;
    bool operator>=(const PathTile& rhs) const;

private:
    World::tile_t m_tile;
    uint m_bestCost;
    uint m_x;
    uint m_y;

    uint m_bestX;
    uint m_bestY;
};

} /* namespace pathFind */

#endif /* PATHTILE_H_ */
