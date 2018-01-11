/**
 * File        : ParDivide.cc
 * Description : Parallel divide search implementation. Makes rough
 *               guess of where certain equidistant points in the final
 *               path will be and "commits" them and searches out from
 *               each point (and start and end) in parallel. After
 *               connections have been made it does a final smoothing
 *               stage in case the original guesses are in highly
 *               non-optimal spots
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

const std::string WORLD_DIR = "worlds";
const std::string WORLD_EXT = ".world";

const std::string ALG_NAME = "parBidir";

const uint numThreads = 4;

Point findStart (const World& world, uint numThreadsLeft, const Point& start, const Point& end);

void search (uint id, uint startX, uint startY, uint endX, uint endY,
             std::unordered_set<uint>& tileIdsFound,
             std::vector<std::unordered_map<uint, PathTile>>& expandedTiles,
             pathFind::PathTile& tile, const pathFind::World& world,
             std::mutex& m, bool& finished, bool& iFound);

void searchNeighbor (const Point& adjPoint, const World& world, const PathTile& tile,
    PriorityQueue& openTiles, const std::unordered_map<uint, PathTile>& expandedTiles);

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

    std::vector<std::unordered_map<uint, PathTile>> expandedTiles;
    std::unordered_set<uint> idsFound;
    std::mutex m;
    bool finished = false;
    bool fFound = false;
    bool rFound = false;

    std::vector<std::thread> threads (numThreads);
    for (uint i = 0; i < numThreads; ++i)
    {
        threads[i] = std::thread (search, i, startX, startY, endX, endY, std::ref (idsFound),
                std::ref(expandedTiles), std::ref(fTile), std::cref(world),
                std::ref(m), std::ref(finished), std::ref(fFound))
    }

    for (auto& t : threads)
    {
        t.join ();
    }

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

    writeResults (finalPath, argv[1], ALG_NAME,
            std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);

    return EXIT_SUCCESS;
}

Point findStart (const World& world, uint numThreadsLeft, float sx, float sy, float ex, float ey)
{
    int diffX = static_cast<int> (ex) - sx;
    int diffY = static_cast<int> (ey) - sy;
    uint forwX = static_cast<float> (diffX) / (numThreadsLeft + 1);
    uint forwY = static_cast<float> (diffY) / (numThreadsLeft + 1);
    uint backX = forwX;
    uint backY = forwY;
    // TODO: Handle 0 slope and undefined (divide by zero)
    int slope = diffY / diffX;
    bool ySlope = true;
    if (abs (diffY) < abs (diffX))
    {
        slope = diffX / diffY;
        ySlope = false;
    }

    uint distAlongSlope = 0;
    while (world (forwX, forwY).cost == 0 && world (backX, backY).cost == 0)
    {
        if (distAlongSlope == slope)
        {
            distAlongSlope = 0;
            if (ySlope)
            {
                currX++;
            }
            else
            {
                currY++;
            }
        }
        else
        {
            distAlongSlope++;
            if (ySlope)
            {
                currY++;
            }
            else
            {
                currX++;
            }
        }
    }
}

void search (uint id, uint startX, uint startY, uint endX, uint endY,
             std::unordered_set<uint>& tileIdsFound,
             std::vector<std::unordered_map<uint, PathTile>>& expandedTiles,
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

        expandedTiles[id][tile.getTile ().id] = tile;
        openTiles.pop ();

        // Check each neighbor
        Point adjPoint {tile.xy ().x + 1, tile.xy ().y}; // east
        searchNeighbor (adjPoint, world, tile, openTiles, expandedTiles[id]);

        adjPoint = {tile.xy ().x, tile.xy ().y + 1}; // south
        if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
        {
            World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
            if (worldTile.cost != 0 &&
                expandedTiles.find (worldTile.id) == expandedTiles.end ())
            {
                openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
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
            }
        }
    }
}


void searchNeighbor (const Point& adjPoint, const World& world, const PathTile& tile,
    PriorityQueue& openTiles, const std::unordered_map<uint, PathTile>& expandedTiles)
{
    if (adjPoint.x < world.getWidth() && adjPoint.y < world.getHeight ())
    {
        World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
        if (worldTile.cost != 0 &&
            expandedTiles.find (worldTile.id) == expandedTiles.end ())
        {
            openTiles.tryUpdateBestCost (worldTile, adjPoint, tile);
        }
    }
}
