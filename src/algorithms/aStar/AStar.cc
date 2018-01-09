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

const std::string WORLD_DIR = "../worlds";
const std::string WORLD_EXT = ".world";

const std::string ALG_NAME = "aStar";

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

    #ifdef GEN_STATS
        std::vector<std::unordered_map<uint, StatPoint>> stats (1);
    #endif

    auto t1 = std::chrono::high_resolution_clock::now();

    // Priority Queue with A* heuristic function added
    PriorityQueue openTiles (world.getWidth (), world.getHeight (),
        [endX, endY] (uint x, uint y)
        {
            return  (x < endX ? endX - x : x - endX) +
                    (y < endY ? endY - y : y - endY);
        });

    // A* algorithm
    openTiles.push (world (startX, startY), {startX, startY}, 0);
    #ifdef GEN_STATS
        stats[0][world (startX, startY).id] = StatPoint {startX, startY};
    #endif

    std::unordered_map<uint, PathTile> expandedTiles;
    PathTile tile = openTiles.top();
    while (tile.xy ().x != endX || tile.xy ().y != endY)
    {
        openTiles.pop ();
        expandedTiles[tile.getTile ().id] = tile;
        // Check each neighbor
        Point adjPoint {tile.xy ().x + 1, tile.xy ().y}; // east
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
                #ifdef GEN_STATS
                    auto statIter = stats[0].find (worldTile.id);
                    if (statIter == stats[0].end ())
                    {
                        stats[0][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
                    }
                    else
                    {
                        statIter->second.processCount++;
                    }
                #endif
            }
        }

        adjPoint = {tile.xy ().x, tile.xy ().y + 1}; // south
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
                #ifdef GEN_STATS
                    auto statIter = stats[0].find (worldTile.id);
                    if (statIter == stats[0].end ())
                    {
                        stats[0][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
                    }
                    else
                    {
                        statIter->second.processCount++;
                    }
                #endif
            }
        }

        adjPoint = {tile.xy ().x - 1, tile.xy ().y}; // west
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
                #ifdef GEN_STATS
                    auto statIter = stats[0].find (worldTile.id);
                    if (statIter == stats[0].end ())
                    {
                        stats[0][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
                    }
                    else
                    {
                        statIter->second.processCount++;
                    }
                #endif
            }
        }

        adjPoint = {tile.xy ().x, tile.xy ().y - 1}; // north
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
                #ifdef GEN_STATS
                    auto statIter = stats[0].find (worldTile.id);
                    if (statIter == stats[0].end ())
                    {
                        stats[0][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
                    }
                    else
                    {
                        statIter->second.processCount++;
                    }
                #endif
            }
        }

        tile = openTiles.top ();
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    // Parse results into a stack
    uint totalCost = tile.getBestCost() - tile.getTile().cost;
    std::vector<Point> finalPath;
    while (tile.xy ().x != startX || tile.xy ().y != startY)
    {
        finalPath.emplace_back(tile.xy ());
        tile = expandedTiles[(tile.bestTile ().y * world.getWidth()) + tile.bestTile ().x];
    }
    finalPath.emplace_back(tile.xy ());

    #ifdef GEN_STATS
        writeResults (finalPath, stats, argv[1], ALG_NAME,
                std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);
    #else
        writeResults (finalPath, argv[1], ALG_NAME,
                std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);
    #endif

    return EXIT_SUCCESS;
}
