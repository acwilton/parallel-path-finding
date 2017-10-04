/**
 * File        : PriorityQueue.h
 * Description : An implementation of a priority queue using a binary heap.
 *               Gives the ability to change the priority of an element while
 *               it is in the heap. Modified specificly for pathfinding in a
 *               "world" by holding onto information about a world.
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

#include "algorithms/tools/PathTile.h"
#include "common/World.h"

namespace pathFind
{

class PriorityQueue
{
public:

    PriorityQueue () = delete;
    PriorityQueue (const World& world);

    ~PriorityQueue ();

    void push (const PathTile& element);
    void pop ();
    PathTile top () const;

    void changeBestCost (uint x, uint y, uint bestCost);

    bool isValid (uint x, uint y) const;
    // Assumes that user already checked that (x, y) is valid
    PathTile getPathTile (uint x, uint y) const;

private:

    struct handle_t
    {
        handle_t (PathTile p_tile, uint p_index)
            : tile(p_tile), index(p_index)
        {
        }

        PathTile tile;
        uint index;
    };

    // Get a handle of a PathTile at a specific position. If out
    // of bounds or doesn't exist (is a wall) then nullptr returned
    std::shared_ptr<handle_t> getHandle (uint x, uint y) const;

    uint getLeftChild (uint index) const;
    uint getRightChild (uint index) const;
    uint getParent (uint index) const;

    void downHeap (uint index);
    void upHeap (uint index);
    void makeHeap ();

    size_t m_worldWidth;
    size_t m_worldHeight;

    std::vector<std::shared_ptr<handle_t>> m_heap;
    std::unordered_map<uint, std::shared_ptr<handle_t>> m_hashTable;
};

} /* namespace parPath */

#endif /* PRIORITYQUEUE_H_ */
