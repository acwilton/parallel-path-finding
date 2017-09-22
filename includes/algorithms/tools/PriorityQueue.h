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
#include <iostream>
#include <memory>

#include "algorithms/tools/PathTile.h"

namespace pathFind
{

class PriorityQueue
{
public:
    PriorityQueue ();
    PriorityQueue (const std::vector<std::shared_ptr<PathTile>>& unheapedElements);
    PriorityQueue (std::vector<std::shared_ptr<PathTile>>&& unheapedElements);

    ~PriorityQueue ();

    void push (const PathTile& element);
    void push (PathTile&& element);
    void pop ();
    PathTile top () const;

    void changeBestCost (std::shared_ptr<PathTile> tile, uint bestCost);

private:

    size_t getLeftChild (size_t index) const;
    size_t getRightChild (size_t index) const;
    size_t getParent (size_t index) const;

    void downHeap (size_t index);
    void upHeap (size_t index);
    void makeHeap ();

    std::vector<std::shared_ptr<PathTile>> m_heap;
};

} /* namespace parPath */

#endif /* PRIORITYQUEUE_H_ */
