/**
 * File        : worldGen.cc
 * Description : Generates a random world and stores it into a world file (.world)
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "common/World.h"

typedef unsigned char uchar;

const std::string WORLD_DIR = "worlds";
const std::string WORLD_EXT = ".world";

int main (int args, char* argv[])
{
    // Program should be started with 3 command line parameters for creating
    // a world file
    if (args != 4)
    {
        std::cout << "Incorrect inputs. Usage: <filename> <width> <height>"
                << std::endl;
        return EXIT_FAILURE;
    }

    // File will be created at "worlds/<filename>"
    // TODO - protect against filename already containing an extension.
    // Perhaps exit the program with error message unless the extension
    // is .world
    std::stringstream fileName;
    fileName << WORLD_DIR << "/" << argv[1] << WORLD_EXT;

    std::ofstream worldFile (fileName.str (),
            std::ofstream::out | std::ofstream::binary);

    size_t width;
    try
    {
        width = boost::lexical_cast<size_t> (argv[2]);
    } catch (boost::bad_lexical_cast &e)
    {
        std::cout << "Width failed to convert to a numeric type" << std::endl;
        return EXIT_FAILURE;
    }

    size_t height;
    try
    {
        height = boost::lexical_cast<size_t> (argv[3]);
    } catch (boost::bad_lexical_cast &e)
    {
        std::cout << "Height failed to convert to a numeric type" << std::endl;
        return EXIT_FAILURE;
    }

    pathFind::World world (width, height);
    world.generateMap (0.5f);

    worldFile << world;

    worldFile.close ();

    return EXIT_SUCCESS;
}

/****************************************************************************************************/
// Function Definitions
