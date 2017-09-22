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

    enum Direction {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3
    };

    const uint INF = std::numeric_limits<uint>::max();

    PathTile ();
    PathTile (World::tile_t tile);
    virtual ~PathTile ();

    void setTile (World::tile_t tile);
    World::tile_t getTile () const;

    void setBestCost (uint bestCost);
    uint getBestCost () const;

    void setIndex (uint index);
    uint getIndex () const;

    void setNeighbor (Direction dir, const std::shared_ptr<PathTile>& neighbor);
    std::shared_ptr<PathTile> getNeighbor (Direction dir) const;

    bool operator< (const PathTile& rhs) const;

private:
    World::tile_t m_tile;
    uint m_bestCost;
    std::shared_ptr<PathTile> m_neighbors[4];

    uint m_index;
};

} /* namespace pathFind */

#endif /* PATHTILE_H_ */
