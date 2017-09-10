// World.cc

#include "World.h"

#include <random>
#include <functional>

typedef unsigned char uchar;

namespace parPath
{
World::World (size_t height, size_t width)
        : m_height (height),
          m_width (width),
          m_tiles (height, std::vector<tile_t> (width,
          { static_cast<uchar> (0) }))
{
    generateMap ();
}

void World::generateMap ()
{
    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    // Starting position in map
    size_t x = gen () % m_width;
    size_t y = gen () % m_height;
    m_tiles[y][x].cost = static_cast<uchar> (1);

    size_t numCarvedTiles = 1;
    size_t targetNumCarvedTiles = (m_width * m_height) / 2;

    // "Drunkard's walk" algorithm for carving out a continuous chunk of the map.
    while (numCarvedTiles < targetNumCarvedTiles)
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
        if (m_tiles[y][x].cost == static_cast<uchar> (0))
        {
            ++numCarvedTiles;
            m_tiles[y][x].cost = static_cast<uchar> (1);
        }
    }

}

World::tile_t World::operator() (size_t row, size_t column)
{
    return m_tiles[row][column];
}

std::ostream& operator<< (std::ostream& stream, const World& world)
{
    stream << world.m_height << world.m_width;
    for (auto& row : world.m_tiles)
    {
        for (auto& t : row)
        {
            //stream << e.cost;
            stream << static_cast<uchar> (t.cost);
        }
    }
    return stream;
}

std::istream& operator>> (std::istream& stream, World& world)
{
    size_t height, width;
    stream >> height >> width;
    std::vector<std::vector<World::tile_t>> tiles (height, std::vector(width));

    for (auto& row : tiles)
    {
        for (auto& t : row)
        {
            stream >> t.cost;
        }
    }

    return stream;
}

} /* namespace parPath */
