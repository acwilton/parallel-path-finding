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

template <typename T, typename Compare = std::less<T>>
namespace parPath
{

class PriorityQueue
{
public:
    PriorityQueue ();
    virtual ~PriorityQueue ();

private:

};

} /* namespace parPath */

#endif /* PRIORITYQUEUE_H_ */
