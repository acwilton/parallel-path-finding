/*
 * PathTile.cc
 */

#include <limits>

#include "algorithms/tools/PathTile.h"

typedef unsigned int uint;

namespace pathFind
{

PathTile::PathTile ()
    : m_bestCost (INF),
      m_x (0),
      m_y (0)
{
}

PathTile::PathTile (World::tile_t tile)
    : m_tile (tile),
      m_bestCost (INF),
      m_x (0),
      m_y (0)
{
}

PathTile::PathTile (const PathTile& other)
    : m_tile (other.m_tile),
      m_bestCost (other.m_bestCost),
      m_x (other.m_x),
      m_y (other.m_y)
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

void PathTile::setBestCost (uint bestCost)
{
    m_bestCost = bestCost;
}

uint PathTile::getBestCost () const
{
    return m_bestCost;
}

void PathTile::setX (uint x)
{
    m_x = x;
}

uint PathTile::getX () const
{
    return m_x;
}

void PathTile::setY (uint y)
{
    m_y = y;
}

uint PathTile::getY () const
{
    return m_y;
}

uint PathTile::getID () const
{
    return m_tile.id;
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
