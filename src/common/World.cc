// World.cc

#include "common/World.h"

#include <random>
#include <functional>
#include <cmath>

typedef unsigned int uint;
typedef unsigned char uchar;

namespace parPath
{

World::World ()
        : m_height (0),
          m_width (0)
{
}

World::World (size_t height, size_t width)
        : m_height (height),
          m_width (width)
{
}

void World::generateMap ()
{
    m_tiles = std::vector<std::vector<tile_t>>(m_height,
            std::vector<tile_t> (m_width, { static_cast<uchar> (0) }));

    std::random_device rd;
    std::minstd_rand0 gen (rd ());

    // Starting position in map
    uint xMiddle = ceil (static_cast<float>(m_width) / 2);
    uint yMiddle = ceil (static_cast<float>(m_height) / 2);
    size_t x = gen () % xMiddle + (xMiddle / 2);
    size_t y = gen () % yMiddle + (yMiddle / 2);
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
            m_tiles[y][x].cost = static_cast<uchar> ((gen () % 255) + 1);
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

World::tile_t World::operator() (size_t row, size_t column)
{
    return m_tiles[row][column];
}

std::ostream& operator<< (std::ostream& stream, const World& world)
{
    stream << world.m_height << "\n" << world.m_width << "\n";
    for (auto& row : world.m_tiles)
    {
        for (auto& t : row)
        {
            //stream << e.cost;
            stream << static_cast<uchar> (t.cost);
        }
    }
    stream << "\n";
    return stream;
}

std::istream& operator>> (std::istream& stream, World& world)
{
    stream >> world.m_height >> world.m_width;
    stream.ignore (1);
    world.m_tiles.resize(world.m_height);

    for (auto& row : world.m_tiles)
    {
        row.resize (world.m_width);
        for (auto& tile : row)
        {
            tile.cost = stream.get ();
        }
    }

    return stream;
}

} /* namespace parPath */
