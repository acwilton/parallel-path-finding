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
                   const std::string& algName);

void readResults (std::vector<Point>& path, const Point& start, const Point& end,
                  const std::string& worldName, const std::string& algName);

} /* namespace pathFind */

#endif /* RESULTS_H */
