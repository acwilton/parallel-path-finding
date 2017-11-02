/**
 * File        : AStar.cc
 * Description : Implementation of A* algorithm using a specified "world"
 *               from the worlds folder.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <chrono>

#include <boost/lexical_cast.hpp>

#include "algorithms/tools/PathTile.h"
#include "algorithms/tools/PriorityQueue.h"
#include "common/Results.h"

using namespace pathFind;

const std::string WORLD_DIR = "worlds";
const std::string WORLD_EXT = ".world";

const std::string ALG_NAME = "bidir";

int main (int args, char* argv[])
{
    // Program should be started with 5 command line parameter
    // that specifies the name of the world file to read from,
    // the start x, start y, end x, and end y
    if (args != 6 )
    {
        std::cout << "Incorrect inputs. Usage: <filename> <start x> <start y> <end x> <end y>" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse the world file
    std::stringstream filename;
    filename << WORLD_DIR << "/" << argv[1] << WORLD_EXT;

    std::ifstream worldFile (filename.str (),
            std::ifstream::in | std::ifstream::binary);

    if (!worldFile)
    {
        std::cout << "World file doesn't exist." << std::endl;
        return EXIT_FAILURE;
    }
    pathFind::World world;

    worldFile >> world;

    // Parse the start and end points
    uint startX, startY, endX, endY;
    try
    {
        startX = boost::lexical_cast<uint> (argv[2]);
        startY = boost::lexical_cast<uint> (argv[3]);
        endX = boost::lexical_cast<uint> (argv[4]);
        endY = boost::lexical_cast<uint> (argv[5]);
    } catch (boost::bad_lexical_cast &e)
    {
        std::cout << "Start and end points failed to convert to numeric types" << std::endl;
        return EXIT_FAILURE;
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    // Priority Queue with A* heuristic function added
    PriorityQueue forwardOpenTiles (world.getWidth (), world.getHeight (), [endX, endY] (uint x, uint y)
        {
            return  (x < endX ? endX - x : x - endX) +
                    (y < endY ? endY - y : y - endY);
        });

    PriorityQueue reverseOpenTiles (world.getWidth (), world.getHeight (), [startX, startY] (uint x, uint y)
        {
            return  (x < startX ? startX - x : x - startX) +
                    (y < startY ? startY - y : y - startY);
        });

    // A* algorithm
    forwardOpenTiles.push (world (startX, startY), {startX, startY}, 0);
    reverseOpenTiles.push (world (endX, endY), {endX, endY}, 0);

    std::unordered_map<uint, PathTile> expandedTiles;
    PathTile fTile = forwardOpenTiles.top ();
    PathTile rTile = reverseOpenTiles.top ();
    while ((fTile.xy ().x != endX || fTile.xy ().y != endY) &&
            (rTile.xy ().x != startX || rTile.xy ().y != startY))
    {
        // forward search
        fTile = forwardOpenTiles.top ();
        auto overlapTile = expandedTiles.find(fTile.getTile ().id);
        if (overlapTile != expandedTiles.end ())
        {
            // Best path found
            rTile = overlapTile->second;
            break;
        }
        forwardOpenTiles.pop ();
        expandedTiles[fTile.getTile ().id] = fTile;

        // Check each neighbor
        Point adjPoint {fTile.xy ().x + 1, fTile.xy ().y}; // east
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                forwardOpenTiles.tryUpdateBestCost (worldTile, adjPoint, fTile);
            }
        }

        adjPoint = {fTile.xy ().x, fTile.xy ().y + 1}; // south
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                forwardOpenTiles.tryUpdateBestCost (worldTile, adjPoint, fTile);
            }
        }

        adjPoint = {fTile.xy ().x - 1, fTile.xy ().y}; // west
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                forwardOpenTiles.tryUpdateBestCost (worldTile, adjPoint, fTile);
            }
        }

        adjPoint = {fTile.xy ().x, fTile.xy ().y - 1}; // north
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                forwardOpenTiles.tryUpdateBestCost (worldTile, adjPoint, fTile);
            }
        }

        // reverse search
        rTile = reverseOpenTiles.top ();
        overlapTile = expandedTiles.find(rTile.getTile ().id);
        if (overlapTile != expandedTiles.end ())
        {
            fTile = overlapTile->second;
            // Best path found
            break;
        }
        reverseOpenTiles.pop ();
        expandedTiles [rTile.getTile ().id] = rTile;

        // Check each neighbor
        adjPoint = {rTile.xy ().x + 1, rTile.xy ().y}; // east
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                reverseOpenTiles.tryUpdateBestCost (worldTile, adjPoint, rTile);
            }
        }

        adjPoint = {rTile.xy ().x, rTile.xy ().y + 1}; // south
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                reverseOpenTiles.tryUpdateBestCost (worldTile, adjPoint, rTile);
            }
        }

        adjPoint = {rTile.xy ().x - 1, rTile.xy ().y}; // west
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                reverseOpenTiles.tryUpdateBestCost (worldTile, adjPoint, rTile);
            }
        }

        adjPoint = {rTile.xy ().x, rTile.xy ().y - 1}; // north
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                reverseOpenTiles.tryUpdateBestCost (worldTile, adjPoint, rTile);
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    // Parse reverse results
    uint totalCost = 0;
    std::vector <Point> reversePath;
    while (rTile.xy ().x != endX || rTile.xy ().y != endY)
    {
        totalCost += rTile.getTile().cost;
        reversePath.emplace_back (rTile.xy ());
        rTile = expandedTiles[(rTile.bestTile ().y * world.getWidth ()) + rTile.bestTile ().x];
    }
    reversePath.emplace_back (rTile.xy ());

    std::vector<Point> finalPath (reversePath.rbegin (), reversePath.rend ());
    while (fTile.xy ().x != startX || fTile.xy ().y != startY)
    {
        fTile = expandedTiles[(fTile.bestTile ().y * world.getWidth()) + fTile.bestTile ().x];
        totalCost += fTile.getTile().cost;
        finalPath.emplace_back(fTile.xy ());
    }
    totalCost -= fTile.getTile().cost;

    writeResults (finalPath, argv[1], ALG_NAME,
            std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);

    return EXIT_SUCCESS;
}
