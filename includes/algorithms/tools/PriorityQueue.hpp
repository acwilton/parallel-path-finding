/**
 * File        : PriorityQueue.h
 * Description : An implementation of a priority queue using a binary heap.
 *               Gives the ability to change the priority of an element while
 *               it is in the heap.
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include <vector>
#include <functional>


namespace pathFind
{

template <typename T, typename Compare = std::less<T>>
class PriorityQueue
{
public:
    PriorityQueue ()
    {

    }

    virtual ~PriorityQueue ()
    {

    }

private:
    int x;
};

} /* namespace parPath */

#endif /* PRIORITYQUEUE_H_ */
