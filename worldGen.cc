/**
 * File        : worldGen.cc
 * Description : Generates a random world and stores it into a world file (.world)
 */

/****************************************************************************************************/
// Global includes

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <boost/lexical_cast.hpp>

/****************************************************************************************************/
// Local includes

#include "world.h"

/****************************************************************************************************/
// Using and typedef statements

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::stringstream;
using std::ofstream;

using namespace parPath;

typedef unsigned char uchar;

/****************************************************************************************************/
// Function declaration

/****************************************************************************************************/
// Constants

const string worldDir = "worlds";
const string worldExt = ".world";

/****************************************************************************************************/
// Main

int main(int args, char* argv[])
{
    // Program should be started with 3 command line parameters for creating
    // a world file
    if (args != 4)
    {
	cout << "Incorrect inputs. Usage: <filename> <width> <height>" << endl;
	return EXIT_FAILURE;
    }

    // File will be created at "worlds/<filename>"
    // TODO - protect against filename already containing an extension.
    // Perhaps exit the program with error message unless the extension
    // is .world
    stringstream fileName;
    fileName << worldDir << "/" << argv[1] << worldExt;

    ofstream worldFile (fileName.str(), ofstream::out | ofstream::binary);

    size_t width;
    try
    {
	width = boost::lexical_cast<size_t>(argv[2]);
    }
    catch (boost::bad_lexical_cast &e)
    {
	cout << "Width failed to convert to a numeric type" << endl;
	return EXIT_FAILURE;
    }

    size_t height;
    try
    {
	height = boost::lexical_cast<size_t>(argv[3]);
    }
    catch (boost::bad_lexical_cast &e)
    {
	cout << "Height failed to convert to a numeric type" << endl;
	return EXIT_FAILURE;
    }

    World world (height, width);

    worldFile.close();
    
    return EXIT_SUCCESS;
}

/****************************************************************************************************/
// Function Definitions

