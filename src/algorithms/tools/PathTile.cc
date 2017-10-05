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
      m_y (0),
      m_bestX (0),
      m_bestY (0)
{
}

PathTile::PathTile (World::tile_t tile, uint x, uint y)
    : m_tile (tile),
      m_bestCost (INF),
      m_x (x),
      m_y (y),
      m_bestX (x),
      m_bestY (y)
{
}

PathTile::PathTile (const PathTile& other)
    : m_tile (other.m_tile),
      m_bestCost (other.m_bestCost),
      m_x (other.m_x),
      m_y (other.m_y),
      m_bestX (other.m_bestX),
      m_bestY (other.m_bestY)
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
        m_x = rhs.m_x;
        m_y = rhs.m_y;
        m_bestX = rhs.m_bestX;
        m_bestY = rhs.m_bestY;
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

void PathTile::setBestCost (uint bestCost)
{
    m_bestCost = bestCost;
}

uint PathTile::getBestCost () const
{
    return m_bestCost;
}

uint PathTile::x () const
{
    return m_x;
}

uint PathTile::y () const
{
    return m_y;
}

uint PathTile::bestX () const
{
    return m_bestX;
}

uint PathTile::bestY () const
{
    return m_bestY;
}

void PathTile::setBestTile (uint x, uint y)
{
    m_bestX = x;
    m_bestY = y;
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
