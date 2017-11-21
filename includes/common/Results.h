/**
 * File        : Results.h
 * Description : A few functions that read and write path result files as well
 *               as path performance files
 */

#ifndef RESULTS_H
#define RESULTS_H

#include <iostream>
#include <vector>
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

inline void writeResults (const std::vector<Point>& path, const std::string& worldName,
                          const std::string& algName, uint ms, uint totalCost)
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

inline void readResults (std::vector<Point>& path, const Point& start, const Point& end,
                         const std::string& worldName, const std::string& algName)
{
    std::stringstream fileName;
    fileName << RESULTS_DIR << "/" << worldName << "_"
             << start.x << "_" << start.y << "_"
             << end.x << "_" << end.y
             << "/" << algName << RESULTS_EXT;

    std::ifstream resultFile (fileName.str ());
    if (!resultFile)
    {
        std::cout << "Failed to open file: " << fileName.str () << std::endl;
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
}
} /* namespace pathFind */

#endif /* RESULTS_H */
