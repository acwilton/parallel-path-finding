/*
 * PathTile.cc
 */

#include <limits>

#include "algorithms/tools/PathTile.h"

typedef unsigned int uint;

namespace pathFind
{

PathTile::PathTile ()
    : m_xy (0, 0),
      m_bestCost (INF),
      m_bestTile (0, 0)
{
}

PathTile::PathTile (World::tile_t tile, const Point& xy)
    : m_tile (tile),
      m_xy (xy.x, xy.y),
      m_bestCost (INF),
      m_bestTile (xy.x, xy.y)
{
}

PathTile::PathTile (const PathTile& other)
    : m_tile (other.m_tile),
      m_xy (other.m_xy.x, other.m_xy.y),
      m_bestCost (other.m_bestCost),
      m_bestTile (other.m_bestTile.x, other.m_bestTile.y)
{
}

PathTile::~PathTile ()
{
}

PathTile& PathTile::operator= (const PathTile& rhs)
{
    if (this != &rhs)
    {
        m_tile = rhs.m_tile;
        m_bestCost = rhs.m_bestCost;
        m_xy = rhs.m_xy;
        m_bestTile = rhs.m_bestTile;
    }
    return *this;
}

void PathTile::setTile (World::tile_t tile)
{
    m_tile = tile;
}

World::tile_t PathTile::getTile () const
{
    return m_tile;
}

Point PathTile::xy () const
{
    return m_xy;
}

void PathTile::setBestCost (uint bestCost)
{
    m_bestCost = bestCost;
}

uint PathTile::getBestCost () const
{
    return m_bestCost;
}

Point PathTile::bestTile () const
{
    return m_bestTile;
}

void PathTile::setBestTile (const Point& tile_xy)
{
    m_bestTile = tile_xy;
}

bool PathTile::operator< (const PathTile& rhs) const
{
    return m_bestCost < rhs.m_bestCost;
}

bool PathTile::operator>= (const PathTile& rhs) const
{
    return !(*this < rhs);
}

} /* namespace pathFind */
