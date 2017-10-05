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
    Point (uint x, uint y)
        : x (x), y (y)
    {
    }

    Point (const Point& other)
        : x (other.x), y (other.y)
    {
    }

    Point& operator= (const Point& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }
};

} /* namespace pathFind */

#endif /* POINT_H */
