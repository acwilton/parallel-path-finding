/**
 * File        : Results.h
 * Description : A few functions that read and write path result files as well
 *               as path performance files
 */

#ifndef RESULTS_H
#define RESULTS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "common/Point.h"

namespace pathFind
{

const std::string RESULTS_DIR = "../results";
const std::string RESULTS_EXT = ".res";
const std::string PERFORMANCE_EXT = ".perf";
const std::string STAT_EXT = ".stat";

struct StatPoint
{
    StatPoint (uint x = 0, uint y = 0, uint processCount = 1)
        :   tile (x, y),
            processCount (processCount)
    {
    }
    Point tile;
    uint processCount;
};

inline void writeResults (const std::vector<Point>& path,
                          const std::vector<std::unordered_map<uint, StatPoint>>& stats,
                          const std::string& worldName, const std::string& algName, uint ms,
                          uint totalCost)
{
    std::stringstream dirName;
    dirName << RESULTS_DIR << "/" << worldName << "_"
            << path.back ().x << "_" << path.back ().y << "_"
            << path.front ().x << "_" << path.front ().y;
    boost::filesystem::create_directory(dirName.str ());

    std::ofstream resultFile (dirName.str () + "/" + algName + RESULTS_EXT);
    resultFile << path.size() << std::endl;
    for (auto ri = path.rbegin(); ri != path.rend(); ++ri)
    {
        resultFile << ri->x << " " << ri->y << std::endl;
    }
    resultFile << "Total Cost: " << totalCost << std::endl;

    resultFile.close();

    std::ofstream performanceFile (dirName.str () + "/" + algName + PERFORMANCE_EXT);
    performanceFile << "time: " << ms << std::endl;

    std::stringstream threadInfo;

    std::unordered_map<uint, StatPoint> combinedStats;
    uint totalSpread = 0;
    uint totalWork = 0;
    for (uint i = 0; i < stats.size (); ++i)
    {
        threadInfo << "thread " << std::to_string (i) << std::endl;
        threadInfo << "----------" << std::endl;
        totalSpread += stats[i].size ();
        threadInfo << "tile spread: " << stats[i].size () << std::endl;
        uint threadTotalWork = 0;
        for (const auto& s : stats[i])
        {
            threadTotalWork += s.second.processCount;
            combinedStats[s.first] = s.second;
        }
        totalWork += threadTotalWork;
        threadInfo << "total work done: " << threadTotalWork << std::endl << std::endl;
    }

    performanceFile << "total spread: " << combinedStats.size () << std::endl;
    performanceFile << "average thread spread: "
                    << static_cast<float> (totalSpread) / stats.size () << std::endl;
    performanceFile << "total work: " << totalWork << std::endl;
    performanceFile << "work per thread: " << static_cast<float> (totalWork) / stats.size ()
                    << std::endl << std::endl;
    performanceFile << threadInfo.rdbuf();
    performanceFile.close ();

    std::ofstream statFile (dirName.str () + "/" + algName + STAT_EXT);
    uint threadCount = 0;
    for (const auto& thread : stats)
    {
        statFile << "thread " << threadCount << "\n";
        ++threadCount;
        for (const auto& s : thread)
        {
            statFile << s.first << " " << s.second.tile.x << " " << s.second.tile.y << " " << s.second.processCount << "\n";
        }
    }
    statFile.close ();
}

inline void writeResults (const std::vector<Point>& path,
                          const std::string& worldName, const std::string& algName, uint ms, uint totalCost)
{
    std::stringstream dirName;
    dirName << RESULTS_DIR << "/" << worldName << "_"
            << path.back ().x << "_" << path.back ().y << "_"
            << path.front ().x << "_" << path.front ().y;
    boost::filesystem::create_directory(dirName.str ());

    std::ofstream resultFile (dirName.str () + "/" + algName + RESULTS_EXT);
    resultFile << path.size() << std::endl;
    for (auto ri = path.rbegin(); ri != path.rend(); ++ri)
    {
        resultFile << ri->x << " " << ri->y << std::endl;
    }
    resultFile << "Total Cost: " << totalCost << std::endl;

    resultFile.close();

    std::ofstream performanceFile (dirName.str () + "/" + algName + PERFORMANCE_EXT);
    performanceFile << "time: " << ms;
    performanceFile.close ();
}

inline bool readResults (std::vector<Point>& path, std::vector<std::unordered_map<uint, StatPoint>>& stats,
                         uint& maxProcessCount, const Point& start, const Point& end,
                         const std::string& worldName, const std::string& algName)
{
    std::stringstream fileName;
    fileName << RESULTS_DIR << "/" << worldName << "_"
             << start.x << "_" << start.y << "_"
             << end.x << "_" << end.y
             << "/" << algName;

    std::ifstream resultFile (fileName.str () + RESULTS_EXT);
    if (!resultFile)
    {
        return false;
    }

    uint size;
    resultFile >> size;

    path.reserve (size);

    for (uint i = 0; i < size; ++i)
    {
        uint x, y;
        resultFile >> x >> y;
        path.emplace_back (x, y);
    }

    resultFile.close();

    std::ifstream statFile (fileName.str () + STAT_EXT);
    if (!statFile)
    {
        std::cout << "No stat file.\n";
        return true;
    }
    maxProcessCount = 0;
    uint currentThread = 0;
    std::string token;
    while (statFile >> token)
    {
        if (token == "thread")
        {
            statFile >> currentThread;
            stats.resize (stats.size () + 1);
        }
        else
        {
            uint id, x, y, processCount;
            id = std::stoi (token);
            statFile >> x >> y >> processCount;
            if (processCount > maxProcessCount)
            {
                maxProcessCount = processCount;
            }
            stats[currentThread][id] = StatPoint {x, y, processCount};
        }
    }
    statFile.close ();

    return true;
}
} /* namespace pathFind */

#endif /* RESULTS_H */
