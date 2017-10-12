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
#include "common/Point.h"

typedef unsigned int uint;

namespace pathFind
{

class PathTile
{
public:

    const uint INF = std::numeric_limits<uint>::max();

    PathTile ();
    PathTile (World::tile_t tile, const Point& xy);
    PathTile (const PathTile& other);
    virtual ~PathTile ();

    PathTile& operator= (const PathTile& rhs);

    void setTile (World::tile_t tile);
    World::tile_t getTile () const;

    Point xy () const;

    void setBestCost (uint bestCost);
    uint getBestCost () const;

    Point bestTile () const;
    void setBestTile (const Point& tile_xy);

    bool operator< (const PathTile& rhs) const;
    bool operator>=(const PathTile& rhs) const;

private:
    World::tile_t m_tile;
    Point m_xy;

    uint m_bestCost;
    Point m_bestTile;
};

} /* namespace pathFind */

#endif /* PATHTILE_H_ */
