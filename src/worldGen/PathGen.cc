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
const std::string PATH_EXT = ".path";

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

    uint maxThreshold = world.getWidth() + world.getHeight ();
    // Find Start Point
    pathFind::Point start;
    start.x = 0;
    start.y = 0;
    for (uint i = 1; i < maxThreshold; ++i)
    {
        bool foundStart = false;
        pathFind::Point temp = start;
        while(temp.x < world.getWidth () && temp.y < world.getHeight())
        {
            if (world(temp.x, temp.y).cost != 0)
            {
                foundStart = true;
                break;
            }
            --temp.x;
            ++temp.y;
        }

        if (foundStart)
        {
            start = temp;
            break;
        }

        if (i < world.getWidth())
        {
            ++start.x;
        }
        else
        {
            ++start.y;
        }
    }

    pathFind::Point end;
    end.x = world.getWidth() - 1;
    end.y = world.getHeight() - 1;
    for (uint i = 1; i < maxThreshold; ++i)
    {
        bool foundEnd = false;
        pathFind::Point temp = end;
        while(temp.x < world.getWidth () && temp.y < world.getHeight())
        {
            if (world(temp.x, temp.y).cost != 0)
            {
                foundEnd = true;
                break;
            }
            --temp.x;
            ++temp.y;
        }

        if (foundEnd)
        {
            end = temp;
            break;
        }

        if (i < world.getHeight())
        {
            --end.y;
        }
        else
        {
            --end.x;
        }
    }

    std::stringstream pathFileName;
    pathFileName << argv[1] << PATH_EXT;
    std::ofstream pathFile (pathFileName.str ());

    pathFile << start.x << std::endl
        << start.y << std::endl
        << end.x << std::endl
        << end.y;

    return EXIT_SUCCESS;
}
