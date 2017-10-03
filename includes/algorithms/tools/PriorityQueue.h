/**
 * File        : PriorityQueue.h
 * Description : An implementation of a priority queue using a binary heap.
 *               Gives the ability to change the priority of an element while
 *               it is in the heap.
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include <vector>
#include <unordered_map>
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
    PriorityQueue (const World& world);
    PriorityQueue (const std::vector<PathTile>& unheapedElements);
    PriorityQueue (std::vector<std::shared_ptr<PathTile>>&& unheapedElements);

    ~PriorityQueue ();

    void push (const PathTile& element);
    void push (std::shared_ptr<PathTile>& element);
    void pop ();
    PathTile top () const;

    void changeBestCost (std::shared_ptr<PathTile> tile, uint bestCost);
    void changeBestCost (uint id, uint bestCost);

    std::shared_ptr<PathTile> getPathTile (uint id) const;

private:

    struct handle_t
    {
        handle_t (std::shared_ptr<PathTile> p_tilePtr, uint p_index)
            : tilePtr(p_tilePtr), index(p_index)
        {
        }
        PathTile& tile ()
        {
            return *tilePtr;
        }

        std::shared_ptr<PathTile> tilePtr;
        uint index;
    };

    uint getLeftChild (uint) const;
    uint getRightChild (uint index) const;
    uint getParent (uint index) const;

    void downHeap (uint index);
    void upHeap (uint index);
    void makeHeap ();

    std::vector<std::shared_ptr<handle_t>> m_heap;
    std::unordered_map<uint, std::shared_ptr<handle_t>> m_hashTable;
};

} /* namespace parPath */

#endif /* PRIORITYQUEUE_H_ */
