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
#include <deque>
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

const std::string ALG_NAME = "parFringe_" + std::to_string(NUMBER_OF_THREADS);

const uint numThreads = NUMBER_OF_THREADS;

#ifdef GEN_STATS
    std::vector<std::unordered_map<uint, StatPoint>> stats (numThreads);
#endif

uint
heuristic (uint x, uint y, uint endX, uint endY);

/*
void search (uint id, uint numThreads, uint startX, uint startY, uint endX, uint endY,
             std::vector<PathTile>& now, std::vector<std::vector<PathTile>>& later,
             tbb::concurrent_unordered_map<uint, PathTile>& closedTiles,
             std::vector<std::unordered_map<uint, PathTile>>& seen,
             std::vector<uint>& mins, uint threshold, const pathFind::World& world,
             bool& finished, std::mutex& finishedLock, boost::barrier& syncPoint);
*/
void search (uint id, uint endX, uint endY,
             /*std::deque<PathTile>& now,*/ std::vector<std::deque<PathTile>>& localNow,
             std::vector<std::deque<PathTile>>& later,
             tbb::concurrent_unordered_map<uint, bool>& closedTiles,
             std::vector<std::unordered_map<uint, PathTile>>& seen,
             uint threshold, const pathFind::World& world,
             bool& finished, std::mutex& finishedLock, boost::barrier& syncPoint);

 void
 searchNeighbor(const Point& adjPoint, uint id, uint threshold, const PathTile& current, const World& world, uint endX, uint endY,
 		 tbb::concurrent_unordered_map<uint, bool>& closedTiles, std::vector<std::unordered_map<uint, PathTile>>& seen,
 		 std::vector<std::deque<PathTile>>& later, std::vector<std::deque<PathTile>>& localNow);

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

    // Setup
    //std::vector<PathTile> now;
    uint startHeuristic = (startX < endX ? endX - startX : startX - endX) +
            			  (startY < endY ? endY - startY : startY - endY);

    std::vector<std::deque<PathTile>> localNow (numThreads);
    localNow[0].emplace_back (world (startX, startY), Point{startX, startY},
                        Point {startX, startY}, 0, startHeuristic);

    std::vector<std::deque<PathTile>> later (numThreads);
    std::vector<std::unordered_map<uint, PathTile>> seen (numThreads);
    std::vector<uint> mins (numThreads);
    tbb::concurrent_unordered_map<uint, bool> closedTiles;

    boost::barrier syncPoint (numThreads);
    std::mutex finishedLock;
    bool finished = false;

    std::vector<std::thread> threads(numThreads);
    for (uint i = 0; i < numThreads; ++i)
    {
    	threads[i] = std::thread (search, i, endX, endY, std::ref (localNow), //std::ref(now),
    			std::ref(later), std::ref(closedTiles), std::ref (seen), startHeuristic,
    			std::cref(world), std::ref(finished), std::ref (finishedLock), std::ref(syncPoint));
    }

    for (auto& t : threads)
    {
    	t.join ();
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    PathTile endTile;

    bool found = false;
    for (uint i = 0; i < numThreads && !found; ++i)
    {
    	auto seenTile = seen[i].find((endY * world.getWidth()) + endX);
    	if (seenTile != seen[i].end ())
    	{
    		found = true;
    		endTile = seenTile->second;
    	}
    }
    if (!found)
    {
    	std::cout << "ERROR\n";
    	return EXIT_FAILURE;
    }

    // Parse reverse results
    uint totalCost = endTile.getBestCost() - endTile.getTile().cost;
    std::vector<Point> finalPath;
    while (endTile.xy ().x != startX || endTile.xy ().y != startY)
    {
        finalPath.emplace_back(endTile.xy ());
        found = false;
        for (uint i = 0; i < numThreads && !found; ++i)
        {
            auto seenTile = seen[i].find((endTile.bestTile ().y * world.getWidth()) + endTile.bestTile ().x);
            if (seenTile != seen[i].end ())
            {
                found = true;
                endTile = seenTile->second;
            }
        }
        if (!found)
        {
            std::cout << "ERROR\n";
        }
    }
    finalPath.emplace_back(endTile.xy ());

    #ifdef GEN_STATS
        writeResults (finalPath, stats, argv[1], ALG_NAME,
                std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);
    #else
        writeResults (finalPath, argv[1], ALG_NAME,
                std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);
    #endif

    return EXIT_SUCCESS;
}

uint
heuristic (uint x, uint y, uint endX, uint endY)
{
	return  (x < endX ? endX - x : x - endX) +
			(y < endY ? endY - y : y - endY);
}

void search (uint id, uint endX, uint endY,
             /*std::deque<PathTile>& now,*/ std::vector<std::deque<PathTile>>& localNow,
             std::vector<std::deque<PathTile>>& later,
             tbb::concurrent_unordered_map<uint, bool>& closedTiles,
             std::vector<std::unordered_map<uint, PathTile>>& seen,
             uint threshold, const pathFind::World& world,
             bool& finished, std::mutex& finishedLock, boost::barrier& syncPoint)
{
    uint maxTileCost = std::ceil(static_cast<float>(world.getMaxTileCost()));
    bool found = false;

    while (!found)
    {
        //mins[id] = PathTile::INF;
        while (!localNow[id].empty ())
        {
            PathTile current = localNow[id].front();
            localNow[id].pop_front ();
            #ifdef GEN_STATS
                auto statIter = stats[id].find (current.getTile ().id);
                if (statIter == stats[id].end ())
                {
                    stats[id][current.getTile ().id] = StatPoint {current.xy ().x, current.xy ().y};
                }
                else
                {
                    statIter->second.processCount++;
                }
            #endif

            // We have already seen and proccessed this tile. No need to continue.
            /*if (seen[id].find(current.getTile().id) != seen[id].end() || closedTiles.find (current.getTile().id) != closedTiles.end())
            {
                continue;
            }*/

            if (current.getCombinedHeuristic () > threshold)
            {
                //mins[id] = std::min(current.getCombinedHeuristic (), mins[id]);
                later[id].push_back(current);
                continue;
            }

            if (current.xy().x == endX && current.xy().y == endY)
            {
                finishedLock.lock();
                finished = true;
                finishedLock.unlock();
                break;
            }

            Point adjPoint {current.xy ().x + 1, current.xy ().y}; // east
            searchNeighbor (adjPoint, id, threshold, current, world, endX, endY, closedTiles, seen, later, localNow);

            adjPoint = {current.xy ().x, current.xy ().y + 1}; // south
            searchNeighbor (adjPoint, id, threshold, current, world, endX, endY, closedTiles, seen, later, localNow);

            adjPoint = {current.xy ().x - 1, current.xy ().y}; // west
            searchNeighbor (adjPoint, id, threshold, current, world, endX, endY, closedTiles, seen, later, localNow);

            adjPoint = {current.xy ().x, current.xy ().y - 1}; // north
            searchNeighbor (adjPoint, id, threshold, current, world, endX, endY, closedTiles, seen, later, localNow);
        }

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
                //now.clear ();
                uint idx = 0;
                for (auto& v : later)
                {
                    for (auto& i : v)
                	{
                        localNow[idx].emplace_back (i);
                        ++idx;
                        if (idx >= numThreads)
                        {
                            idx = 0;
                        }
                	}
                }
                for (uint i = 0; i < numThreads; ++i)
                {
                    later[i].clear ();
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

void
searchNeighbor(const Point& adjPoint, uint id, uint threshold, const PathTile& current, const World& world, uint endX, uint endY,
		 tbb::concurrent_unordered_map<uint, bool>& closedTiles, std::vector<std::unordered_map<uint, PathTile>>& seen,
		 std::vector<std::deque<PathTile>>& later, std::vector<std::deque<PathTile>>& localNow)
{
	if (adjPoint.x < world.getWidth () && adjPoint.y < world.getHeight ())
	{
		World::tile_t worldTile = world (adjPoint.x, adjPoint.y);
		if (worldTile.cost != 0)
		{
			auto seenTileIter = seen[id].find(worldTile.id);
			if (seenTileIter == seen[id].end ())
			{
				// If we haven't seen the tile then we need to make sure that no one else had either
				if (closedTiles.find(worldTile.id) == closedTiles.end())
				{
                    closedTiles[worldTile.id] = true;
                    uint costToTile = current.getBestCost () + worldTile.cost;
                    if (costToTile > threshold)
                    {

                        later[id].emplace_back (worldTile, adjPoint, current.xy(),
						    costToTile, heuristic (adjPoint.x, adjPoint.y, endX, endY));
                        seen[id][worldTile.id] = later[id].back ();
                    }
                    else
                    {
        				// If no one has then we say that we have.
        				// WARNING: This is indeed a data race. Hopefully one that is ok and breaks nothing but be careful.
    					localNow[id].emplace_back (worldTile, adjPoint, current.xy(),
		                    costToTile, heuristic (adjPoint.x, adjPoint.y, endX, endY));
                        seen[id][worldTile.id] = localNow[id].back ();
                    }
				}
			}
			/*else
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
			}*/
		}
	}
}
