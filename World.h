/**
 * File        : world.h
 * Description : The World class is meant to be the place in which path-finding takes place in.
 *               The Tile struct is also defined in the class and just represents one
 *               location in the world
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include <cstddef>
#include <fstream>

typedef unsigned char uchar;

namespace parPath
{

/**
 * A World object is what represents the space in which the path-finding algorithms will
 * take place. It is essentially a grid of tiles with an associated cost for each. Some
 * tiles may not be crossable and are meant to represent some kind of barrier (like a wall)
 */
class World
{
public:

    /**
     * Constructs a world object with the specified width and height.
     * Values are randomly assigned.
     * @param height   The height of the world.
     * @param width    The width of the world.
     */
    World(size_t height, size_t width);

    // We do not want copying to take place.
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    /**
     * Struct meant to represent a tile in the world where path-finding takes place.
     * Each tile has a cost to enter, but some may not be able to be entered in which
     * case the cost has no meaning and is set to zero.
     */
    struct tile_t
    {
        uchar cost;
        bool allowed;
    };

    /**
     * Returns the tile at the designated column and row
     * @param row      The row we are retrieving from. (y-axis)
     * @param column   The column we are retrieving from. (x-axis)
     * @return The tile at the specified position in the world is returned.
     */
    tile_t
    operator()(size_t row, size_t column);

    friend std::ostream& operator<<(std::ostream& stream, const World& world);

private:
    std::vector<std::vector<tile_t>> m_tiles;
};

} /* namespace parPath */

#endif /* WORLD_H_ */
