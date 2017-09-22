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
      m_neighbors {nullptr, nullptr, nullptr, nullptr},
      m_index (0)
{
}

PathTile::PathTile (World::tile_t tile)
    : m_tile (tile),
      m_bestCost (INF),
      m_neighbors {nullptr, nullptr, nullptr, nullptr},
      m_index (0)
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

void PathTile::setIndex (uint index)
{
    m_index = index;
}

uint PathTile::getIndex () const
{
    return m_index;
}

void PathTile::setNeighbor (Direction dir, const std::shared_ptr<PathTile>& neighbor)
{
    m_neighbors[static_cast<uint> (dir)] = neighbor;
}

std::shared_ptr<PathTile> PathTile::getNeighbor (Direction dir) const
{
    return m_neighbors[static_cast<uint> (dir)];
}

bool PathTile::operator< (const PathTile& rhs) const
{
    return m_bestCost < rhs.m_bestCost;
}

} /* namespace pathFind */
