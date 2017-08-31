// World.cc

#include "World.h"

#include <random>
#include <functional>


typedef unsigned char uchar;

namespace parPath
{
    World::World (size_t height, size_t width)
	: m_tiles (height, vector<tile_t>(width))
    {
	std::random_device rd;
	std::mt19937 gen (rd());
	std::uniform_int_distribution<uchar> ucharDis(1, 255);
	std::discrete_distribution<bool> boolDis{1, 2};
	
	auto costGen = std::bind (ucharDis, gen);
	auto allowedGen = std::bind (boolDis, gen);
	
	for (auto& row : m_tiles)
	{
	    for (auto& e : row)
	    {
		e.cost = costGen();
		e.allowed = allowedGen();
	    }
	}
    }

    World::tile_t
    World::operator() (size_t row, size_t column)
    {
	return m_tiles[row][column];
    }

    ostream&
    operator<< (ostream& stream, const World& world)
    {
	for (auto& row : world.m_tiles)
	{
	    for (auto& e : row)
	    {
		if (e.allowed)
		{
		    stream << e.cost;
		}
		else
		{
		    stream << static_cast<uchar> (0);
		}
	    }
	}
	return stream;
    }
} /* namespace parPath */
