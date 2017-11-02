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
#include <thread>
#include <mutex>

#include <boost/lexical_cast.hpp>

#include "algorithms/tools/PathTile.h"
#include "algorithms/tools/PriorityQueue.h"
#include "common/Results.h"

using namespace pathFind;

const std::string WORLD_DIR = "worlds";
const std::string WORLD_EXT = ".world";

const std::string ALG_NAME = "parBidir";

void search (uint startX, uint startY, uint endX, uint endY,
             std::unordered_map<uint, PathTile>& expandedTiles,
             pathFind::PathTile& tile, const pathFind::World& world,
             std::mutex& m, bool& finished, bool& iFound);

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

    pathFind::PathTile fTile, rTile;

    std::unordered_map<uint, PathTile> expandedTiles;
    std::mutex m;
    bool finished;
    bool fFound = false;
    bool rFound = false;

    std::thread forward (search, startX, startY, endX, endY,
            std::ref(expandedTiles), std::ref(fTile), std::cref(world),
            std::ref(m), std::ref(finished), std::ref(fFound));
    std::thread reverse (search, endX, endY, startX, startY,
            std::ref(expandedTiles), std::ref(rTile), std::cref(world),
            std::ref(m), std::ref(finished), std::ref(rFound));

    forward.join ();
    reverse.join ();

    if (fFound)
    {
        rTile = expandedTiles.at (fTile.getTile ().id);
    }
    else
    {
        fTile = expandedTiles.at (rTile.getTile ().id);
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

void search (uint startX, uint startY, uint endX, uint endY,
             std::unordered_map<uint, PathTile>& expandedTiles,
             pathFind::PathTile& tile, const pathFind::World& world,
             std::mutex& m, bool& finished, bool& iFound)
{
    PriorityQueue openTiles (world.getWidth (), world.getHeight (), [endX, endY] (uint x, uint y)
    {
        return  (x < endX ? endX - x : x - endX) +
                (y < endY ? endY - y : y - endY);
    });

    openTiles.push (world (startX, startY), {startX, startY}, 0);

    tile = openTiles.top ();
    while (tile.xy ().x != endX || tile.xy ().y != endY)
    {
        tile = openTiles.top ();

        m.lock ();
        if (finished)
        {
            m.unlock ();
            break;
        }
        auto overlapTile = expandedTiles.find(tile.getTile ().id);
        if (overlapTile != expandedTiles.end ())
        {
            // Best path found
            iFound = true;
            finished = true;
            m.unlock ();
            break;
        }
        expandedTiles[tile.getTile ().id] = tile;
        m.unlock ();

        openTiles.pop ();

        // Check each neighbor
        Point adjPoint {tile.xy ().x + 1, tile.xy ().y}; // east
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            m.lock ();
            bool notFound = expandedTiles.find (worldTile.id) == expandedTiles.end ();
            m.unlock ();
            if (worldTile.cost != 0 &&
                notFound)
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
            }
        }

        adjPoint = {tile.xy ().x, tile.xy ().y + 1}; // south
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            m.lock ();
            bool notFound = expandedTiles.find (worldTile.id) == expandedTiles.end ();
            m.unlock ();
            if (worldTile.cost != 0 &&
                notFound)
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
            }
        }

        adjPoint = {tile.xy ().x - 1, tile.xy ().y}; // west
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            m.lock ();
            bool notFound = expandedTiles.find (worldTile.id) == expandedTiles.end ();
            m.unlock ();
            if (worldTile.cost != 0 &&
                notFound)
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
            }
        }

        adjPoint = {tile.xy ().x, tile.xy ().y - 1}; // north
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            m.lock ();
            bool notFound = expandedTiles.find (worldTile.id) == expandedTiles.end ();
            m.unlock ();
            if (worldTile.cost != 0 &&
                notFound)
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
            }
        }
    }
}


