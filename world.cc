/**
 * File        : world.cc
 * Description : Class definition of the world class.
 */

/****************************************************************************************************/
// Global includes

#include <random>

/****************************************************************************************************/
// Local includes

#include "world.h"

/****************************************************************************************************/
// Using and typedef statements

typedef unsigned char uchar;

/****************************************************************************************************/

namespace parPath
{
    World::World (size_t height, size_t width)
	: m_tiles (height, vector<tile_t>(width))
    {
	std::random_device rd;
	std::mt19937 gen (rd());
	std::uniform_int_distribution<> dis(1, 255);

	for (auto& row : m_tiles)
	{
	    for (auto& e : row)
	    {
		e.cost = static_cast<uchar>(dis (gen));
	    }
	}
    }

    World::tile_t
    World::operator() (size_t row, size_t column)
    {
	return m_tiles[row][column];
    }
}
