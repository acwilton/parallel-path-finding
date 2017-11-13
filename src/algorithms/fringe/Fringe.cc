/**
 * File        : AStar.cc
 * Description : Implementation of A* algorithm using a specified "world"
 *               from the worlds folder.
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

#include "algorithms/tools/PathTile.h"
#include "common/Results.h"

using namespace pathFind;

const std::string WORLD_DIR = "worlds";
const std::string WORLD_EXT = ".world";

const std::string ALG_NAME = "fringe";
/*
void search (uint startX, uint startY, uint endX, uint endY, std::unordered_set<uint>& tileIdsFound,
             std::unordered_map<uint, PathTile>& expandedTiles,
             pathFind::PathTile& tile, const pathFind::World& world,
             std::mutex& m, bool& finished, bool& iFound);
*/

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

    std::function<uint (uint, uint)> h = [endX, endY] (uint x, uint y)
    {
        return  (x < endX ? endX - x : x - endX) +
                (y < endY ? endY - y : y - endY);
    };

    std::list<PathTile> now, later;
    now.emplace_front (world (startX, startY), Point{startX, startY},
    		Point {startX, startY}, 0, h (startX, startY));

    uint threshold = now.front().getCombinedHeuristic();
    std::unordered_map <uint, PathTile> seen;
    seen[now.front().getTile().id] = now.front ();

    bool found = false;
    PathTile endTile;

    while (!found)
    {
        uint min = PathTile::INF;

        while (!now.empty ())
        {
            PathTile current = now.front();
            now.pop_front();

            if (current.getCombinedHeuristic () > threshold)
            {
                min = std::min(current.getCombinedHeuristic (), min);
                later.push_back(current);
                continue;
            }

            if (current.xy().x == endX && current.xy().y == endY)
            {
                found = true;
                endTile = current;
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
                        seen[current.getTile ().id] = current;
                        now.emplace_front (worldTile, adjPoint, current.xy(),
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
                                min = std::min(current.getCombinedHeuristic (), min);
                                later.push_back(current);
                            }
                            else
                            {
                                now.push_front(seenTile);
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
                        seen[current.getTile ().id] = current;
                        now.emplace_front (worldTile, adjPoint, current.xy(),
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
                                min = std::min(current.getCombinedHeuristic (), min);
                                later.push_back(current);
                            }
                            else
                            {
                                now.push_front(seenTile);
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
                        seen[current.getTile ().id] = current;
                        now.emplace_front (worldTile, adjPoint, current.xy(),
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
                                min = std::min(current.getCombinedHeuristic (), min);
                                later.push_back(current);
                            }
                            else
                            {
                                now.push_front(seenTile);
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
                        seen[current.getTile ().id] = current;
                        now.emplace_front (worldTile, adjPoint, current.xy(),
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
                                min = std::min(current.getCombinedHeuristic (), min);
                                later.push_back(current);
                            }
                            else
                            {
                                now.push_front(seenTile);
                            }
                        }
                    }
                }
            }
        }
        threshold = min;
        now.splice(now.end(), std::move (later));
    }

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

/*
void search (uint startX, uint startY, uint endX, uint endY, std::unordered_set<uint>& tileIdsFound,
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
*/

