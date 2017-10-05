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

const std::string RESULTS_DIR = "results";
const std::string RESULTS_EXT = ".res";

void writeResults (const std::vector<Point>& path, const std::string& worldName,
                   const std::string& algName)
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
}

} /* namespace pathFind */

#endif /* RESULTS_H */
