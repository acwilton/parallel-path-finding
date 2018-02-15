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
    pathFind::Point topLeft;
    topLeft.x = 0;
    topLeft.y = 0;
    for (uint i = 1; i < maxThreshold; ++i)
    {
        bool foundTopLeft = false;
        pathFind::Point temp = topLeft;
        while(temp.x < world.getWidth () && temp.y < world.getHeight())
        {
            if (world(temp.x, temp.y).cost != 0)
            {
                foundTopLeft = true;
                break;
            }
            --temp.x;
            ++temp.y;
        }

        if (foundTopLeft)
        {
            topLeft = temp;
            break;
        }

        if (i < world.getWidth())
        {
            ++topLeft.x;
        }
        else
        {
            ++topLeft.y;
        }
    }

    pathFind::Point topRight;
    topRight.x = world.getWidth() - 1;
    topRight.y = 0;
    for (uint i = 1; i < maxThreshold; ++i)
    {
        bool foundTopRight = false;
        pathFind::Point temp = topRight;
        while(temp.x < world.getWidth () && temp.y < world.getHeight())
        {
            if (world(temp.x, temp.y).cost != 0)
            {
                foundTopRight = true;
                break;
            }
            ++temp.x;
            ++temp.y;
        }

        if (foundTopRight)
        {
            topRight = temp;
            break;
        }

        if (i < world.getWidth())
        {
            --topRight.x;
        }
        else
        {
            ++topRight.y;
        }
    }

    pathFind::Point botLeft;
    botLeft.x = 0;
    botLeft.y = world.getHeight() - 1;
    for (uint i = 1; i < maxThreshold; ++i)
    {
        bool foundBotLeft = false;
        pathFind::Point temp = botLeft;
        while(temp.x < world.getWidth () && temp.y < world.getHeight())
        {
            if (world(temp.x, temp.y).cost != 0)
            {
                foundBotLeft = true;
                break;
            }
            ++temp.x;
            ++temp.y;
        }

        if (foundBotLeft)
        {
            botLeft = temp;
            break;
        }

        if (i < world.getHeight())
        {
            --botLeft.y;
        }
        else
        {
            ++botLeft.x;
        }
    }

    pathFind::Point botRight;
    botRight.x = world.getWidth() - 1;
    botRight.y = world.getHeight() - 1;
    for (uint i = 1; i < maxThreshold; ++i)
    {
        bool foundBotRight = false;
        pathFind::Point temp = botRight;
        while(temp.x < world.getWidth () && temp.y < world.getHeight())
        {
            if (world(temp.x, temp.y).cost != 0)
            {
                foundBotRight = true;
                break;
            }
            --temp.x;
            ++temp.y;
        }

        if (foundBotRight)
        {
            botRight = temp;
            break;
        }

        if (i < world.getHeight())
        {
            --botRight.y;
        }
        else
        {
            --botRight.x;
        }
    }

    pathFind::Point start, end;
    start = topLeft;
    end = botRight;
    uint maxDist = abs (static_cast<int> (start.x) - static_cast<int> (end.x))
        + abs (static_cast<int> (start.y) - static_cast<int> (end.y));

    uint trtl = abs (static_cast<int> (topRight.x) - static_cast<int> (topLeft.x))
        + abs (static_cast<int> (topRight.y) - static_cast<int> (topLeft.y));
    if (trtl > maxDist)
    {
        maxDist = trtl;
        end = topRight;
    }

    uint bltl = abs (static_cast<int> (botLeft.x) - static_cast<int> (topLeft.x))
        + abs (static_cast<int> (botLeft.y) - static_cast<int> (topLeft.y));
    if (bltl > maxDist)
    {
        maxDist = bltl;
        end = botLeft;
    }

    uint brtr = abs (static_cast<int> (botRight.x) - static_cast<int> (topRight.x))
        + abs (static_cast<int> (botRight.y) - static_cast<int> (topRight.y));
    if (brtr > maxDist)
    {
        maxDist = brtr;
        start = botRight;
        end = topRight;
    }

    uint brbl = abs (static_cast<int> (botRight.x) - static_cast<int> (botLeft.x))
        + abs (static_cast<int> (botRight.y) - static_cast<int> (botLeft.y));
    if (brbl > maxDist)
    {
        maxDist = brbl;
        start = botRight;
        end = botLeft;
    }

    uint bltr = abs (static_cast<int> (botLeft.x) - static_cast<int> (topRight.x))
        + abs (static_cast<int> (botLeft.y) - static_cast<int> (topRight.y));
    if (bltr > maxDist)
    {
        maxDist = bltr;
        start = botLeft;
        end = topRight;
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
