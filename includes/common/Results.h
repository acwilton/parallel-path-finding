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

inline void writeResults (const std::vector<Point>& path, const std::unordered_map<uint, StatPoint>& stats,
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

    std::ofstream statFile (dirName.str () + "/" + algName + STAT_EXT);
    for (const auto& s : stats)
    {
        statFile << s.first << " " << s.second.tile.x << " " << s.second.tile.y << " " << s.second.processCount << "\n";
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

inline void readResults (std::vector<Point>& path, std::unordered_map<uint, StatPoint>& stats,
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
        std::cout << "Failed to open file: " << fileName.str () + RESULTS_EXT << std::endl;
        return;
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
        return;
    }
    maxProcessCount = 0;
    while (statFile)
    {
        uint id, x, y, processCount;
        statFile >> id >> x >> y >> processCount;
        if (processCount > maxProcessCount)
        {
            maxProcessCount = processCount;
        }
        stats[id] = StatPoint {x, y, processCount};
    }
    statFile.close ();
}
} /* namespace pathFind */

#endif /* RESULTS_H */
