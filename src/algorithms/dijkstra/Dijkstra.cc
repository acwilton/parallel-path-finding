/**
 * File        : Dijkstra.cc
 * Description : Implementation of djikstra's algorithm using a specified "world"
 *               from the worlds folder.
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include "algorithms/tools/PathTile.h"
#include "algorithms/tools/PriorityQueue.h"

const std::string WORLD_DIR = "worlds";
const std::string WORLD_EXT = ".world";

int main (int args, char* argv[])
{
    // Program should be started with 1 command line parameter
    // that specifies the name of the world file to read from
    if (args != 2)
    {
        std::cout << "Incorrect inputs. Usage: <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    std::stringstream filename;
    filename << WORLD_DIR << "/" << argv[1] << WORLD_EXT;

    std::ifstream worldFile (filename.str (),
            std::ifstream::in | std::ifstream::binary);

    pathFind::World world;

    worldFile >> world;

    std::vector<int> test;
    for (size_t y = 0; y < world.getHeight (); ++y)
    {
        for (size_t x = 0; x < world.getWidth (); ++x)
        {
            if (static_cast<int> (world (y, x).cost) != 0 )
            {
                test.emplace_back(static_cast<int> (world (y, x).cost));
            }
        }
    }

    pathFind::PriorityQueue unvisitedTiles;

    return EXIT_SUCCESS;
}
