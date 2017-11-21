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

#include "tbb/concurrent_unordered_map.h"

#include "algorithms/tools/PathTile.h"
#include "common/Results.h"

using namespace pathFind;

const std::string WORLD_DIR = "../worlds";
const std::string WORLD_EXT = ".world";

const std::string ALG_NAME = "parFringe";

/*
void search (uint id, uint numThreads, uint startX, uint startY, uint endX, uint endY,
             std::vector<PathTile>& now, std::vector<std::vector<PathTile>>& later,
             tbb::concurrent_unordered_map<uint, PathTile>& closedTiles,
             std::vector<std::unordered_map<uint, PathTile>>& seen,
             std::vector<uint>& mins, uint threshold, const pathFind::World& world,
             bool& finished, std::mutex& finishedLock, boost::barrier& syncPoint);
*/
void search (uint id, uint numThreads, uint endX, uint endY,
             std::vector<PathTile>& now, std::vector<std::vector<PathTile>>& later,
             tbb::concurrent_unordered_map<uint, PathTile>& closedTiles,
             std::vector<std::unordered_map<uint, PathTile>>& seen,
             uint threshold, const pathFind::World& world,
             bool& finished, std::mutex& finishedLock, boost::barrier& syncPoint);

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

    const uint numThreads = 4;

    std::vector<PathTile> now;
    uint startHeuristic = (startX < endX ? endX - startX : startX - endX) +
            			  (startY < endY ? endY - startY : startY - endY);
    now.emplace_back (world (startX, startY), Point{startX, startY},
                        Point {startX, startY}, 0, startHeuristic);

    std::vector<std::vector<PathTile>> later (numThreads);
    std::vector<std::unordered_map<uint, PathTile>> seen (numThreads);
    std::vector<uint> mins (numThreads);
    tbb::concurrent_unordered_map<uint, PathTile> closedTiles;

    boost::barrier syncPoint (numThreads);
    std::mutex finishedLock;
    bool finished = false;

    std::vector<std::thread> threads(numThreads);
    for (uint i = 0; i < numThreads; ++i)
    {
    	threads[i] = std::thread (search, i, numThreads, endX, endY, std::ref(now),
    			std::ref(later), std::ref(closedTiles), std::ref (seen), startHeuristic,
    			std::cref(world), std::ref(finished), std::ref (finishedLock), std::ref(syncPoint));
    }

    for (auto& t : threads)
    {
    	t.join ();
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    PathTile endTile = closedTiles.at(world (endX, endY).id);

    // Parse reverse results
    uint totalCost = endTile.getBestCost() - endTile.getTile().cost;
    std::vector<Point> finalPath;
    while (endTile.xy ().x != startX || endTile.xy ().y != startY)
    {
        finalPath.emplace_back(endTile.xy ());
        endTile = closedTiles.at((endTile.bestTile ().y * world.getWidth()) + endTile.bestTile ().x);
    }
    finalPath.emplace_back(endTile.xy ());

    writeResults (finalPath, argv[1], ALG_NAME,
            std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);

    return EXIT_SUCCESS;
}

void search (uint id, uint numThreads, uint endX, uint endY,
             std::vector<PathTile>& now, std::vector<std::vector<PathTile>>& later,
             tbb::concurrent_unordered_map<uint, PathTile>& closedTiles,
             std::vector<std::unordered_map<uint, PathTile>>& seen,
             uint threshold, const pathFind::World& world,
             bool& finished, std::mutex& finishedLock, boost::barrier& syncPoint)
{
	// Heuristic function
    std::function<uint (uint, uint)> h = [endX, endY] (uint x, uint y)
    {
        return  (x < endX ? endX - x : x - endX) +
                (y < endY ? endY - y : y - endY);
    };

    uint maxTileCost = world.getMaxTileCost();
    bool found = false;

    while (!found)
    {
        //mins[id] = PathTile::INF;

        uint localN = ceil (static_cast<float> (now.size()) / numThreads);
        uint start = id * localN;
        uint end = std::min ((id + 1) * localN, static_cast<uint>(now.size()));

        if (start <= end)
        {
            std::vector<PathTile> localNow (now.begin() + start, now.begin() + end);

            while (!localNow.empty ())
            {
                PathTile current = localNow.back();
                localNow.pop_back();

                // We have already seen and proccessed this tile. No need to continue.
                if (seen[id].find(current.getTile().id) != seen[id].end() || closedTiles.find (current.getTile().id) != closedTiles.end())
                {
                    continue;
                }

                if (current.getCombinedHeuristic () > threshold)
                {
                    //mins[id] = std::min(current.getCombinedHeuristic (), mins[id]);
                    later[id].push_back(current);
                    continue;
                }

                // We have now know the best cost to this tile
                seen[id][current.getTile ().id] = current;

                if (current.xy().x == endX && current.xy().y == endY)
                {
                        finishedLock.lock();
                    finished = true;
                    finishedLock.unlock();
                    break;
                }

                Point adjPoint {current.xy ().x + 1, current.xy ().y}; // east
                if (adjPoint.x < world.getWidth () && adjPoint.y < world.getHeight ())
                {
                    World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
                    if (worldTile.cost != 0 && closedTiles.find(worldTile.id) == closedTiles.end())
                    {
                        auto seenTileIter = seen[id].find(worldTile.id);
                        if (seenTileIter == seen[id].end ())
                        {
                            localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                      current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
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
                                   // mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
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
                    if (worldTile.cost != 0 && closedTiles.find(worldTile.id) == closedTiles.end())
                    {
                        auto seenTileIter = seen[id].find(worldTile.id);
                        if (seenTileIter == seen[id].end ())
                        {
                            localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                    current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
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
                                    //mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
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
                    if (worldTile.cost != 0 && closedTiles.find(worldTile.id) == closedTiles.end())
                    {
                        auto seenTileIter = seen[id].find(worldTile.id);
                        if (seenTileIter == seen[id].end ())
                        {
                            localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                    current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
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
                                   // mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
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
                    if (worldTile.cost != 0 && closedTiles.find(worldTile.id) == closedTiles.end())
                    {
                        auto seenTileIter = seen[id].find(worldTile.id);
                        if (seenTileIter == seen[id].end ())
                        {
                            localNow.emplace_back (worldTile, adjPoint, current.xy(),
                                    current.getBestCost () + worldTile.cost, h (adjPoint.x, adjPoint.y));
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
                                    //mins[id] = std::min(seenTile.getCombinedHeuristic (), mins[id]);
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
        }

        closedTiles.insert (seen[id].begin(), seen[id].end ());
        seen[id].clear();
        // Start pushing all of the tiles you have seen into the shared container of closed tiles

        /*for (auto i = seen[id].begin(); i != seen[id].end(); ++i)
        {
        	closedTiles[i->first] = i->second;
        }*/
        syncPoint.wait();
        if (finished)
        {
            found = true;
        }
        else
        {
            if (id == 0)
            {
            	// Merge the later lists into the now list
                now.clear ();
                for (auto& v : later)
                {
                	for (auto& i : v)
                	{
                	    now.emplace_back(i);
                	}
                	later[id].clear ();
                }
            }
            // Find the minimum of the minimums
            threshold += maxTileCost;//mins[0];
            /*for (uint i = 1; i < numThreads; ++i)
            {
                    if (mins[i] < threshold)
                    {
                              threshold = mins[i];
                    }
            }*/
        }
        syncPoint.wait();
    }
}

