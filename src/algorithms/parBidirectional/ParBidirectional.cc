/**
 * File        : ParBidirectional.cc
 * Description : Implementation of the parallel bidirectional A* algorithm
 *               using a specified "world" from the worlds folder.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
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

const std::string WORLD_DIR = "../worlds";
const std::string WORLD_EXT = ".world";
const std::string PATH_EXT = ".path";

const std::string ALG_NAME = "parBidir";

void search (uint startX, uint startY, uint endX, uint endY, std::unordered_set<uint>& tileIdsFound,
             std::unordered_map<uint, PathTile>& expandedTiles,
             pathFind::PathTile& tile, const pathFind::World& world,
             std::mutex& m, bool& finished, bool& iFound
#ifdef GEN_STATS
             ,uint id
#endif
         );

#ifdef GEN_STATS
    std::vector<std::unordered_map<uint, StatPoint>> stats (2);
#endif

int main (int args, char* argv[])
{
    // Program should be started with 5 command line parameters (or 1)
    // that specifies the name of the world file to read from and then optionallys
    // the start x, start y, end x, and end y
    if (args != 6  && args != 2)
    {
        std::cout << "Incorrect inputs. Usage: <filename> (start x) (start y) (end x) (end y)" << std::endl;
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
    worldFile.close ();

    uint startX, startY, endX, endY;

    if (args == 6)
    {
        // Parse the start and end points
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
    }
    else
    {
        std::stringstream pathFilename;
        pathFilename << WORLD_DIR << "/" << argv[1] << PATH_EXT;
        std::ifstream pathIn (pathFilename.str ());
        if (!pathIn)
        {
            std::string pathCommand = "./pathGen " + std::string (argv[1]);
            system (pathCommand.c_str());
            pathIn.close ();
            pathIn.open (pathFilename.str ());
            if (!pathIn)
            {
                std::cout << "Could not construct path." << std::endl;
                return EXIT_FAILURE;
            }
        }
        pathIn >> startX >> startY >> endX >> endY;

    }

    auto t1 = std::chrono::high_resolution_clock::now();

    pathFind::PathTile fTile, rTile;

    std::unordered_map<uint, PathTile> forwardExpandedTiles;
    std::unordered_map<uint, PathTile> reverseExpandedTiles;
    std::unordered_set<uint> idsFound;
    std::mutex m;
    bool finished = false;
    bool fFound = false;
    bool rFound = false;

    std::thread forward (search, startX, startY, endX, endY, std::ref (idsFound),
            std::ref(forwardExpandedTiles), std::ref(fTile), std::cref(world),
            std::ref(m), std::ref(finished), std::ref(fFound)
#ifdef GEN_STATS
            ,0
#endif
        );
    std::thread reverse (search, endX, endY, startX, startY, std::ref(idsFound),
            std::ref(reverseExpandedTiles), std::ref(rTile), std::cref(world),
            std::ref(m), std::ref(finished), std::ref(rFound)
#ifdef GEN_STATS
            ,1
#endif
        );

    forward.join ();
    reverse.join ();

    if (fFound)
    {
        rTile = reverseExpandedTiles.at (fTile.getTile ().id);
    }
    else
    {
        fTile = forwardExpandedTiles.at (rTile.getTile ().id);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    // Parse reverse results
    uint totalCost = 0;
    std::vector <Point> reversePath;
    while (rTile.xy ().x != endX || rTile.xy ().y != endY)
    {
        totalCost += rTile.getTile().cost;
        reversePath.emplace_back (rTile.xy ());
        rTile = reverseExpandedTiles[(rTile.bestTile ().y * world.getWidth ()) + rTile.bestTile ().x];
    }
    reversePath.emplace_back (rTile.xy ());

    std::vector<Point> finalPath (reversePath.rbegin (), reversePath.rend ());
    while (fTile.xy ().x != startX || fTile.xy ().y != startY)
    {
        fTile = forwardExpandedTiles[(fTile.bestTile ().y * world.getWidth()) + fTile.bestTile ().x];
        totalCost += fTile.getTile().cost;
        finalPath.emplace_back(fTile.xy ());
    }
    totalCost -= fTile.getTile().cost;

    #ifdef GEN_STATS
        writeResults (finalPath, stats, argv[1], ALG_NAME,
                std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);
    #else
        writeResults (finalPath, argv[1], ALG_NAME,
                std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);
    #endif

    return EXIT_SUCCESS;
}

void search (uint startX, uint startY, uint endX, uint endY, std::unordered_set<uint>& tileIdsFound,
             std::unordered_map<uint, PathTile>& expandedTiles,
             pathFind::PathTile& tile, const pathFind::World& world,
             std::mutex& m, bool& finished, bool& iFound
#ifdef GEN_STATS
             ,uint id
#endif
         )
{
    PriorityQueue openTiles (world.getWidth (), world.getHeight (), [endX, endY] (uint x, uint y)
    {
        return  (x < endX ? endX - x : x - endX) +
                (y < endY ? endY - y : y - endY);
    });

    openTiles.push (world (startX, startY), {startX, startY}, 0);
    #ifdef GEN_STATS
        stats[id][world (startX, startY).id] = StatPoint {startX, startY};
    #endif

    tile = openTiles.top ();

    expandedTiles[tile.getTile().id] = tile;

    while (tile.xy ().x != endX || tile.xy ().y != endY)
    {
        tile = openTiles.top ();

        m.lock ();
        if (finished)
        {
            m.unlock ();
            break;
        }
        if (tileIdsFound.count(tile.getTile().id))
        {
            // Best path found
            iFound = true;
            finished = true;
            m.unlock ();
            break;
        }
        tileIdsFound.insert(tile.getTile().id);
        m.unlock ();

        expandedTiles[tile.getTile ().id] = tile;
        openTiles.pop ();

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
                    auto statIter = stats[id].find (worldTile.id);
                    if (statIter == stats[id].end ())
                    {
                        stats[id][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
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
                    auto statIter = stats[id].find (worldTile.id);
                    if (statIter == stats[id].end ())
                    {
                        stats[id][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
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
                    auto statIter = stats[id].find (worldTile.id);
                    if (statIter == stats[id].end ())
                    {
                        stats[id][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
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
                    auto statIter = stats[id].find (worldTile.id);
                    if (statIter == stats[id].end ())
                    {
                        stats[id][worldTile.id] = StatPoint {adjPoint.x, adjPoint.y};
                    }
                    else
                    {
                        statIter->second.processCount++;
                    }
                #endif
            }
        }
    }
}
