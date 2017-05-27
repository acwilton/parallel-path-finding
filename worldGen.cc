/**
 * File        : worldGen.cc
 * Description : Generates a random world and stores it into a world file (.world)
 */

/****************************************************************************************************/
// Global includes

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <sstream>

#include <boost/lexical_cast.hpp

/****************************************************************************************************/
// Local includes


/****************************************************************************************************/
// Using and typedef statements

using std::vector;
using std::cout;
using std::endl;
using std::stringstream;
using std::fstream;

typedef uchar unsigned char

/****************************************************************************************************/
// Function declaration

/****************************************************************************************************/
// Constants

constexpr string worldDir = "worlds";
constexpr string worldExt = ".world";

/****************************************************************************************************/
// Main

int main(int args, char* argv[])
{
    // Program should be started with 3 command line parameters for creating
    // a world file
    if (args != 4)
    {
	cout << "Incorrect inputs. Usage: <filename> <width> <height>" << endl;
    }

    // File will be created at "worlds/<filename>"
    // TODO - protect against filename already containing an extension.
    // Perhaps exit the program with error message unless the extension
    // is .world
    stringstream fileName;
    fileName << worldDir << "/" << argv[1] << worldExt;

    ofstream worldFile (fileName.str(), ofstream::out | ofstream::binary);
    
    vector<vector<uchar>> world;

    mapFile.close();
    
    return EXIT_SUCCESS;
}
