/**
 * File        : Point.h
 * Description : Holds a simple point data struct for holding a 2D position
 *               in world coordinates
 */

#ifndef POINT_H
#define POINT_H

typedef unsigned int uint;

namespace pathFind
{

struct Point
{
    uint x, y;
    Point ()
        : x (0), y (0)
    {

    }
    Point (uint x, uint y)
        : x (x), y (y)
    {
    }
};

} /* namespace pathFind */

#endif /* POINT_H */
