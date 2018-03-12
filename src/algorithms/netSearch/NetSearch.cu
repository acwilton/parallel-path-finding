/**
 * File        : NetSearch.cu
 * Description : Implementation of the novel algorithm, NetSearch, using CUDA.
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <chrono>

#include <boost/lexical_cast.hpp>

#include <cuda.h>

#include "algorithms/tools/PathTile.h"
#include "common/Results.h"

 using namespace pathFind;

const std::string WORLD_DIR = "../worlds";
const std::string WORLD_EXT = ".world";
const std::string PATH_EXT = ".path";

const std::string ALG_NAME = "netSearch";

const size_t BLOCK_WIDTH = 256;
const size_t BLOCK_HEIGHT = 256;

struct ThreadTile
{
    struct Branch
    {
        bool finished = false;
        uint64_t bestCost = std::numeric_limits<uint64_t>::max();
    };

    uint64_t cost = 0;
    Branch bestBranch;
    Branch branches[4];
};

__global__ void
netSearchIteration (ThreadTile* tiles, size_t pitch, size_t width, size_t height)
{
    uint idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint idy = blockIdx.y * blockDim.y + threadIdx.y;

    if (idx < width && idy < height)
    {
        uint64_t pos = idx + idy * pitch;
        if (tiles[pos].cost != 0 && !tiles[pos].bestBranch.finished)
        {
            uint64_t neighborsX[4] = {
                idx,
                idx + 1,
                idx,
                idx - 1
            };

            uint64_t neighborsY[4] = {
                idy - 1,
                idy,
                idy + 1,
                idy
            };

            for (uint i = 0; i < 4; ++i)
            {

                if (neighborsX[i] < width && neighborsY[i] < height)
                {
                    uint64_t neighbor = neighborsX[i] + neighborsY[i] * pitch;
                    tiles[pos].branches[i].bestCost = tiles[neighbor].bestBranch.bestCost + tiles[pos].cost;
                    tiles[pos].branches[i].finished = tiles[neighbor].bestBranch.finished;
                }
                tiles[pos].branches[i].finished = true;
            }
        }
    }
}

__global__ void
threadUpdate (bool* finished, ThreadTile* tiles, size_t pitch, size_t width, size_t height, uint64_t startX, uint64_t startY)
{
    uint idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint idy = blockIdx.y * blockDim.y + threadIdx.y;

    if (idx < width && idy < height)
    {
        uint64_t pos = idx + idy * pitch;
        if (tiles[pos].cost != 0 && !tiles[pos].bestBranch.finished)
        {
            uint64_t min = std::numeric_limits<uint64_t>::max();
            uint64_t minIndex = 4;
            for (uint i = 0; i < 4; ++i)
            {
                if (tiles[pos].branches[i].bestCost < min)
                {
                    min = tiles[pos].branches[i].bestCost;
                    minIndex = i;
                }
            }
            tiles[pos].bestBranch = tiles[pos].branches[minIndex];

            // If start tile knows best path then we can notify the CPU
            if (idx == startX && idy == startY && tiles[pos].bestBranch.finished)
            {
                *finished = true;
            }

        }
    }
}

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

    // Intialize Cuda with flag that allows mapped memory access on device
    cudaSetDeviceFlags (cudaDeviceMapHost);

    std::vector<ThreadTile> h_threadTiles (world.getWidth() * world.getHeight());
    for (size_t x = 0; x < world.getWidth(); ++x)
    {
        for (size_t y = 0; y < world.getHeight(); ++y)
        {
            uint64_t bestCost = world (x, y).cost;
            h_threadTiles[x + y * world.getWidth ()].cost = bestCost;
            if (bestCost == 0)
            {
                bestCost = std::numeric_limits<uint64_t>::max();
            }
            else
            {
                bestCost += (x < endX ? endX - x : x - endX) +
                            (y < endY ? endY - y : y - endY);
            }
            h_threadTiles[x + y * world.getWidth ()].bestBranch.bestCost = bestCost;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    h_threadTiles[endX + endY * world.getWidth ()].bestBranch.finished = true;
    h_threadTiles[endX + endY * world.getWidth ()].bestBranch.bestCost = 0;

    ThreadTile* d_threadTiles;
    size_t pitch;
    cudaMallocPitch (reinterpret_cast<void **> (&d_threadTiles), &pitch, world.getWidth () * sizeof(ThreadTile), world.getHeight ());
    cudaMemcpy2D (d_threadTiles, pitch, h_threadTiles.data (), world.getWidth () * sizeof (ThreadTile),
            world.getWidth () * sizeof (ThreadTile), world.getHeight (), cudaMemcpyHostToDevice);

    // Set up mapped page-locked memory so that the gpu can quickly tell the cpu when a path is found
    bool* h_finished;
    bool* d_finished;
    cudaHostAlloc (reinterpret_cast<void**> (&h_finished), sizeof (bool), cudaHostAllocMapped);
    //cudaMalloc (reinterpret_cast<void**> (&d_finished), sizeof (bool))
    *h_finished = false;
    cudaHostGetDevicePointer (reinterpret_cast<void**> (&d_finished), h_finished, 0);


    dim3 blockSize (BLOCK_WIDTH, BLOCK_HEIGHT);

    // Figure out how many blocks we will need based on how many tiles there are in the world
    size_t gridWidth = (world.getWidth () % BLOCK_WIDTH == 0) ?
                        world.getWidth () / BLOCK_WIDTH :
                        world.getWidth () / BLOCK_WIDTH + 1;
    size_t gridHeight = (world.getHeight () % BLOCK_HEIGHT == 0) ?
                        world.getHeight () / BLOCK_HEIGHT :
                        world.getHeight () / BLOCK_HEIGHT + 1;
    dim3 gridSize (gridWidth, gridHeight);

    uint64_t iter = 0;
    //while (!(*h_finished) && iter < 100000)
    //{
        netSearchIteration <<< gridSize, blockSize >>> (d_threadTiles, pitch, world.getWidth (), world.getHeight ());
        //threadUpdate <<< gridSize, blockSize >>> (d_finished, d_threadTiles, pitch, world.getWidth (), world.getHeight (), startX, startY);
        //cudaDeviceSynchronize ();
        //std::cout << "iter: " << iter << std::endl;
        ++iter;
    //}

    cudaMemcpy2D (h_threadTiles.data (), world.getWidth () * sizeof (ThreadTile), d_threadTiles, pitch,
            world.getWidth () * sizeof(ThreadTile), world.getHeight (), cudaMemcpyDeviceToHost);

    auto t2 = std::chrono::high_resolution_clock::now();

    cudaFreeHost (h_finished);
    cudaFree (d_threadTiles);

    std::vector<Point> finalPath;
    Point tile {startX, startY};
    uint64_t totalCost = h_threadTiles[tile.x + tile.y * world.getWidth ()].bestBranch.bestCost -
                        h_threadTiles[tile.x + tile.y * world.getWidth ()].cost;

    std::cout << "end bestCost: " << h_threadTiles[endX + endY * world.getWidth ()].bestBranch.bestCost
            << " finished: " << h_threadTiles[endX + endY * world.getWidth ()].bestBranch.finished << std::endl;

    std::cout << "endx - 1 bestCost: " << h_threadTiles[endX - 1 + endY * world.getWidth ()].bestBranch.bestCost
            << " finished: " << h_threadTiles[endX - 1 + endY * world.getWidth ()].bestBranch.finished << std::endl;

    std::cout << "endx - 1 branch bestCost: " << h_threadTiles[endX - 1 + endY * world.getWidth ()].branches[1].bestCost
            << " finished: " << h_threadTiles[endX - 1 + endY * world.getWidth ()].branches[1].finished << std::endl;

    std::cout << "endy - 1 bestCost: " << h_threadTiles[endX + (endY - 1) * world.getWidth ()].bestBranch.bestCost
            << " finished: " << h_threadTiles[endX + (endY - 1) * world.getWidth ()].bestBranch.finished << std::endl;
    while (tile.x != endX || tile.y != endY)
    {
        std::cout << "x: " << tile.x << " y: " << tile.y << std::endl;
        finalPath.emplace_back (tile);
        uint64_t id = tile.x + tile.y * world.getWidth ();
        uint64_t bestCost = h_threadTiles[id].bestBranch.bestCost;
        std::cout << "bestCost: " << bestCost << " finished: " << h_threadTiles[id].bestBranch.finished << "\n\n";
        uint64_t bestTile = 4;
        for (uint i = 0; i < 4; ++i)
        {
            if (h_threadTiles[id].branches[i].finished && h_threadTiles[id].branches[i].bestCost == bestCost)
            {
                bestTile = i;
                break;
            }
        }
        switch (bestTile)
        {
        case 0:
            tile.y--;
            break;
        case 1:
            tile.x++;
            break;
        case 2:
            tile.y++;
            break;
        case 3:
            tile.x--;
            break;
        }
    }

    std::vector<Point> reversedFinalPath (finalPath.rbegin (), finalPath.rend());
/*
    // Parse results into a stack
    uint totalCost = tile.getBestCost() - tile.getTile().cost;
    std::vector<Point> finalPath;
    while (tile.xy ().x != startX || tile.xy ().y != startY)
    {
        finalPath.emplace_back(tile.xy ());
        tile = expandedTiles[(tile.bestTile ().y * world.getWidth()) + tile.bestTile ().x];
    }
    finalPath.emplace_back(tile.xy ()); */

    writeResults (reversedFinalPath, argv[1], ALG_NAME,
            std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count(), totalCost);

    return EXIT_SUCCESS;
}
