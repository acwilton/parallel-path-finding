/**
 * File        : PathGen.cc
 * Description : Generates the largest path possible in a given world and stores
 *               it in a temporary .path file
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include "common/World.h"
#include "common/Point.h"

const std::string WORLD_DIR = "../worlds";
const std::string WORLD_EXT = ".world";

int main (int args, char* argv[])
{

    if (args != 2)
    {
        std::cout << "Incorrect inputs. Usage: <world name>"
                << std::endl;
        return EXIT_FAILURE;
    }

    std::stringstream fileName;
    fileName << WORLD_DIR << "/" << argv[1] << WORLD_EXT;

    std::ifstream worldFile (fileName.str (),
            std::ofstream::out | std::ofstream::binary);

    pathFind::World world;
    worldFile >> world;

    uint maxThreshold = world.getWidth() + getHeight ();
    // Find Start Point
    pathFind::Point start;
    for (uint i = 0; i <)

    return EXIT_SUCCESS;
}
