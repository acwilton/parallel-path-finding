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
    for (uint y = 0; y < world.getHeight (); ++y)
    {
        for (uint x = 0; x < world.getWidth (); ++x)
        {
            if (static_cast<int> (world (x, y).cost) != 0 )
            {
                test.emplace_back(static_cast<int> (world (x, y).cost));
            }
        }
    }

    pathFind::PriorityQueue unvisitedTiles (world);
    pathFind::PriorityQueue tmp (world);

    for (uint i = 0; i < world.getNumOpenTiles(); ++i)
    {
        pathFind::PathTile x = unvisitedTiles.top();
        std::cout << "top: " << x.getBestCost() << std::endl;
        unvisitedTiles.pop();
    }

    auto x = tmp.getPathTile (1, 0);
    std::cout << "0 1 BC: " << x.getBestCost();
    std::cout << " x: " << x.x() << " y: " << x.y() << " id: " << x.getID() << std::endl;
    tmp.changeBestCost(x.x(), x.y(), 74);
    auto y = tmp.getPathTile (2, 1);
    std::cout << "1 2 BC: " << y.getBestCost();
    std::cout << " x: " << y.x() << " y: " << y.y() << " id: " << y.getID() << std::endl;
    tmp.changeBestCost (y.x(), y.y(), 21);

    for (uint i = 0; i < world.getNumOpenTiles (); ++i)
    {
        std::cout << "top2: " << tmp.top().getBestCost() << std::endl;
        tmp.pop ();
    }

    return EXIT_SUCCESS;
}
