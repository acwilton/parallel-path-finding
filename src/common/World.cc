// World.cc

#include "common/World.h"

#include <random>
#include <functional>
#include <cmath>

typedef unsigned int uint;
typedef unsigned char uchar;

namespace pathFind
{

World::World ()
        : m_height (0),
          m_width (0),
          m_openTiles (0)
{
}

World::World (size_t height, size_t width)
        : m_height (height),
          m_width (width),
          m_openTiles (0)
{
}

void World::generateMap (float percentCarved)
{
    if (percentCarved > 1.0f)
    {
        percentCarved = 1.0f;
    }

    m_tiles = std::vector<tile_t>(m_height * m_width,
            { static_cast<uchar> (0), 0 });

    for (uint i = 0; i < m_tiles.size (); ++i)
    {
        m_tiles[i].id = i;
    }

    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    // Starting position in map
    uint xMiddle = ceil (static_cast<float>(m_width) / 2);
    uint yMiddle = ceil (static_cast<float>(m_height) / 2);
    uint x = gen () % xMiddle + (xMiddle / 2);
    uint y = gen () % yMiddle + (yMiddle / 2);
    m_tiles[getID (y, x)].cost = static_cast<uchar> ((gen () % 255) + 1);

    size_t numCarvedTiles = 1;
    m_openTiles = (m_width * m_height) * percentCarved;

    // "Drunkard's walk" algorithm for carving out a continuous chunk of the map.
    while (numCarvedTiles < m_openTiles)
    {
        // Pick a random direction to walk in
        switch (gen () % 4)
        {
        case 0:
            if (x < m_width - 1)
            {
                ++x;
            }
            break;
        case 1:
            if (y < m_height - 1)
            {
                ++y;
            }
            break;
        case 2:
            if (x > 0)
            {
                --x;
            }
            break;
        case 3:
        default:
            if (y > 0)
            {
                --y;
            }
            break;
        }

        // If current tile is a "wall" then carve it out into open space
        if (m_tiles[getID (y, x)].cost == static_cast<uchar> (0))
        {
            ++numCarvedTiles;
            m_tiles[getID (y, x)].cost = static_cast<uchar> ((gen () % 255) + 1);
        }
    }

}

size_t World::getWidth () const
{
    return m_width;
}

size_t World::getHeight () const
{
    return m_height;
}

size_t World::getNumOpenTiles() const
{
    return m_openTiles;
}

World::tile_t World::operator() (uint row, uint column) const
{
    return m_tiles[getID (row, column)];
}

uint World::getID(uint y, uint x) const
{
    return (m_width * y) + x;
}

std::vector<World::tile_t>::iterator World::begin ()
{
    return m_tiles.begin ();
}

std::vector<World::tile_t>::const_iterator World::begin () const
{
    return m_tiles.begin ();
}

std::vector<World::tile_t>::iterator World::end ()
{
    return m_tiles.end ();
}

std::vector<World::tile_t>::const_iterator World::end () const
{
    return m_tiles.end ();
}

std::ostream& operator<< (std::ostream& stream, const World& world)
{
    stream << world.m_height << "\n" << world.m_width << "\n";
    for (auto& tile : world.m_tiles)
    {
        stream << static_cast<uchar> (tile.cost);
    }
    stream << "\n";
    return stream;
}

std::istream& operator>> (std::istream& stream, World& world)
{
    stream >> world.m_height >> world.m_width;
    stream.ignore (1);
    world.m_tiles.resize(world.m_height * world.m_width);

    for (uint i = 0; i < world.m_tiles.size (); ++i)
    {
        world.m_tiles[i].cost = stream.get ();
        world.m_tiles[i].id = i;
        if (world.m_tiles[i].cost != static_cast<uchar> (0))
        {
            ++world.m_openTiles;
        }
    }

/*
    for (auto& tile : world.m_tiles)
    {
        tile.cost = stream.get ();
    }
*/
    return stream;
}

} /* namespace pathFind */
