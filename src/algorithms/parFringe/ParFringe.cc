/**
 * File        : ParFringe.cc
 * Description : Implementation of the parallel fringe algorithm using a
 *               specified "world" from the worlds folder.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>

#include <boost/lexical_cast.hpp>
#include <boost/thread/barrier.hpp>

#include "algorithms/tools/PathTile.h"
#include "common/Results.h"

using namespace pathFind;

const std::string WORLD_DIR = "worlds";
const std::string WORLD_EXT = ".world";

const std::string ALG_NAME = "parFringe";

void search (uint startX, uint startY, uint endX, uint endY, std::unordered_set<uint>& tileIdsFound,
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

    std::vector<PathTile> now;
    std::function<uint (uint, uint)> h = [endX, endY] (uint x, uint y)
    {
        return  (x < endX ? endX - x : x - endX) +
                (y < endY ? endY - y : y - endY);
    };
    now.emplace_back (world (startX, startY), Point{startX, startY},
                        Point {startX, startY}, 0, h (startX, startY));

    auto t2 = std::chrono::high_resolution_clock::now();

    // Parse reverse results
    uint totalCost = endTile.getBestCost() - endTile.getTile().cost;
    std::vector<Point> finalPath;
    while (endTile.xy ().x != startX || endTile.xy ().y != startY)
    {
        finalPath.emplace_back(endTile.xy ());
        endTile = seen.at((endTile.bestTile ().y * world.getWidth()) + endTile.bestTile ().x);
    }
    finalPath.emplace_back(endTile.xy ());

    writeResults (finalPath, argv[1], ALG_NAME,
            std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);

    return EXIT_SUCCESS;
}

void search (uint id, uint numThreads, uint startX, uint startY, uint endX, uint endY,
             std::vector<PathTile>& now, std::vector<std::vector<PathTile>>& later,
             std::vector<std::unordered_map<uint, PathTile>>& closedTiles,
             std::vector<uint>& mins, uint threshold, const pathFind::World& world, std::mutex& m,
             boost::barrier& b, bool& finished)
{
    std::function<uint (uint, uint)> h = [endX, endY] (uint x, uint y)
    {
        return  (x < endX ? endX - x : x - endX) +
                (y < endY ? endY - y : y - endY);
    };

    std::unordered_map <uint, PathTile> seen;
    seen[now.back().getTile().id] = now.back ();

    bool found = false;

    while (!found)
    {
        mins[id] = PathTile::INF;

        std::vector<PathTile> localNow (now.begin(), now.end()); // Change obviously

        while (!localNow.empty ())
        {
            PathTile current = localNow.back();
            localNow.pop_back();

            if (current.getCombinedHeuristic () > threshold)
            {
                mins[id] = std::min(current.getCombinedHeuristic (), mins[id]);
                later[id].push_back(current);
                continue;
            }

            if (current.xy().x == endX && current.xy().y == endY)
            {
                m.lock();
                finished = true;
                m.unlock();
                break;
            }


            Point adjPoint {current.xy ().x + 1, current.xy ().y}; // east
            if (adjPoint.x < world.getWidth () && adjPoint.y < world.getHeight ())
            {
                World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
                if (worldTile.cost != 0)
                {
                    auto seenTileIter = seen.find(worldTile.id);
                    if (seenTileIter == seen.end ())
                    {
                    	localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                          current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
                        seen[worldTile.id] = localNow.back();
                    }
                    else
                    {
                        PathTile& seenTile = seenTileIter->second;
                        uint costToTile = current.getBestCost () + seenTile.getTile ().cost;
                        if (seenTile.getBestCost () > costToTile)
                        {
                            seenTile.setBestCost (costToTile);
                            seenTile.setBestTile (current.xy());
                            if (seenTile.getCombinedHeuristic () > threshold)
                            {
                                mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
                                later[id].push_back(seenTile);
                            }
                            else
                            {
                            	localNow.push_back(seenTile);
                            }
                        }
                    }
                }
            }

            adjPoint = {current.xy ().x, current.xy ().y + 1}; // south
            if (adjPoint.x < world.getWidth () && adjPoint.y < world.getHeight ())
            {
                World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
                if (worldTile.cost != 0)
                {
                    auto seenTileIter = seen.find(worldTile.id);
                    if (seenTileIter == seen.end ())
                    {
                    	localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                          current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
                        seen[worldTile.id] = localNow.back();
                    }
                    else
                    {
                        PathTile& seenTile = seenTileIter->second;
                        uint costToTile = current.getBestCost () + seenTile.getTile ().cost;
                        if (seenTile.getBestCost () > costToTile)
                        {
                            seenTile.setBestCost (costToTile);
                            seenTile.setBestTile (current.xy());
                            if (seenTile.getCombinedHeuristic () > threshold)
                            {
                                mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
                                later[id].push_back(seenTile);
                            }
                            else
                            {
                            	localNow.push_back(seenTile);
                            }
                        }
                    }
                }
            }

            adjPoint = {current.xy ().x - 1, current.xy ().y}; // west
            if (adjPoint.x < world.getWidth () && adjPoint.y < world.getHeight ())
            {
                World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
                if (worldTile.cost != 0)
                {
                    auto seenTileIter = seen.find(worldTile.id);
                    if (seenTileIter == seen.end ())
                    {
                    	localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                          current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
                        seen[worldTile.id] = localNow.back();
                    }
                    else
                    {
                        PathTile& seenTile = seenTileIter->second;
                        uint costToTile = current.getBestCost () + seenTile.getTile ().cost;
                        if (seenTile.getBestCost () > costToTile)
                        {
                            seenTile.setBestCost (costToTile);
                            seenTile.setBestTile (current.xy());
                            if (seenTile.getCombinedHeuristic () > threshold)
                            {
                                mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
                                later[id].push_back(seenTile);
                            }
                            else
                            {
                            	localNow.push_back(seenTile);
                            }
                        }
                    }
                }
            }

            adjPoint = {current.xy ().x, current.xy ().y - 1}; // north
            if (adjPoint.x < world.getWidth () && adjPoint.y < world.getHeight ())
            {
                World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
                if (worldTile.cost != 0)
                {
                    auto seenTileIter = seen.find(worldTile.id);
                    if (seenTileIter == seen.end ())
                    {
                    	localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                          current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
                        seen[worldTile.id] = localNow.back();
                    }
                    else
                    {
                        PathTile& seenTile = seenTileIter->second;
                        uint costToTile = current.getBestCost () + seenTile.getTile ().cost;
                        if (seenTile.getBestCost () > costToTile)
                        {
                            seenTile.setBestCost (costToTile);
                            seenTile.setBestTile (current.xy());
                            if (seenTile.getCombinedHeuristic () > threshold)
                            {
                                mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
                                later[id].push_back(seenTile);
                            }
                            else
                            {
                            	localNow.push_back(seenTile);
                            }
                        }
                    }
                }
            }
        }

        b.wait();
        if (id == 0)
        {
        	// Merge the later lists into the now list
            now.clear ();
            for (uint i = 0; i < numThreads; ++i)
            {
            	std::move (later[i].begin (), later[i].end (), std::back_inserter (now));
            }
        }
        if (id == 1)
        {
            // Merge all seen tiles into closed tiles
        }
        if (id == 2)
        {
        	// Find the minimum of the minimums
            threshold = mins[0];
            for (uint i = 1; i < numThreads; ++i)
            {
                if (mins[i] < threshold)
                {
                    threshold = mins[i];
                }
            }
        }
        if (finished)
        {
            found = true;
        }
        b.wait();
    }
}

