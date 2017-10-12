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
      m_heuristic (0),
      m_bestTile (0, 0)
{
}

PathTile::PathTile (World::tile_t tile, const Point& xy, uint heuristic)
    : m_tile (tile),
      m_xy (xy.x, xy.y),
      m_bestCost (INF),
      m_heuristic (heuristic),
      m_bestTile (xy.x, xy.y)
{
}

PathTile::PathTile (const PathTile& other)
    : m_tile (other.m_tile),
      m_xy (other.m_xy.x, other.m_xy.y),
      m_bestCost (other.m_bestCost),
      m_heuristic (other.m_heuristic),
      m_bestTile (other.m_bestTile.x, other.m_bestTile.y)
{
}

PathTile::~PathTile ()
{
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

void PathTile::setHeuristic (uint heuristic)
{
    m_heuristic = heuristic;
}

uint PathTile::getHeuristic () const
{
    return m_heuristic;
}

uint PathTile::getCombinedHeuristic () const
{
    return m_bestCost == INF ? INF : m_bestCost + m_heuristic;
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
    return getCombinedHeuristic () < rhs.getCombinedHeuristic ();
}

bool PathTile::operator>= (const PathTile& rhs) const
{
    return !(*this < rhs);
}

} /* namespace pathFind */
