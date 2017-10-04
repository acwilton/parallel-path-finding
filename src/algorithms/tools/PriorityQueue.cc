/*
 * PriorityQueue.cc
 */

#include "algorithms/tools/PriorityQueue.h"

namespace pathFind
{

PriorityQueue::PriorityQueue(const World& world)
    : m_worldWidth (world.getWidth()),
      m_worldHeight (world.getHeight())
{
    m_heap.reserve(world.getNumOpenTiles ());
    for (uint y = 0; y < m_worldWidth; ++y)
    {
        for (uint x = 0; x < m_worldHeight; ++x)
        {
            World::tile_t t = world (x, y);
            if (t.cost != static_cast<uchar>(0))
            {
                m_heap.emplace_back (std::make_shared<handle_t>(
                        PathTile{t, x, y}, m_heap.size ()));
                m_hashTable[t.id] = m_heap.back ();
            }
        }
    }
    makeHeap();
}

PriorityQueue::~PriorityQueue ()
{
}

void PriorityQueue::push (const PathTile& element)
{
    std::shared_ptr<handle_t> newHandle = std::make_shared<handle_t> (
            element, m_heap.size());
    m_hashTable[element.getID()] = newHandle;
    m_heap.emplace_back(newHandle);
    upHeap (m_heap.size() - 1);
}

void PriorityQueue::pop ()
{
    if (m_heap.empty())
    {
        return;
    }

    m_hashTable.erase (m_heap[0]->tile.getID());
    m_heap[0] = m_heap.back ();
    m_heap.pop_back ();
    if (m_heap.empty())
    {
        return;
    }
    downHeap (0);
}

PathTile PriorityQueue::top () const
{
    return m_heap[0]->tile;
}

void PriorityQueue::changeBestCost (uint x, uint y, uint bestCost)
{
    auto handle = getHandle (x, y);
    if (handle != nullptr)
    {
        handle->tile.setBestCost(bestCost);
        upHeap (handle->index);
    }
}

bool PriorityQueue::isValid (uint x, uint y) const
{
    return getHandle (x, y) != nullptr;
}

PathTile PriorityQueue::getPathTile (uint x, uint y) const
{
    return getHandle(x, y)->tile;
}

std::shared_ptr<PriorityQueue::handle_t> PriorityQueue::getHandle (uint x, uint y) const
{
    std::shared_ptr<handle_t> handle = nullptr;
    if (x < m_worldHeight && y < m_worldWidth)
    {
        auto ptr = m_hashTable.find ((m_worldWidth * y) + x);
        if (ptr != m_hashTable.end ())
        {
            handle = ptr->second;
        }
    }
    return handle;
}

uint PriorityQueue::getLeftChild (uint index) const
{
    return 2 * index + 1;
}

uint PriorityQueue::getRightChild (uint index) const
{
    return 2 * index + 2;
}

uint PriorityQueue::getParent (uint index) const
{
    return ((index - 1) / 2);
}

void PriorityQueue::downHeap (uint index)
{
    auto value = m_heap[index];
    size_t c;
    while ((c = getLeftChild (index)) < m_heap.size ())
    {
        if (c + 1 < m_heap.size () && m_heap[c + 1]->tile < m_heap[c]->tile)
        {
            ++c;
        }

        if (m_heap[c]->tile >= value->tile)
        {
            break;
        }

        // Priority queue is responsible for updating the index for each handle_t
        // if it is changed inside of it's vector. That way when the hash table returns
        // a pointer to it, it will always have the right index into the array for
        // the priority queue to use.
        m_heap[index] = m_heap[c];
        m_heap[index]->index = index;
        index = c;
    }
    m_heap[index] = value;
    m_heap[index]->index = index;
}

void PriorityQueue::upHeap (uint index)
{
    auto value = m_heap[index];
    while (index != 0)
    {
        size_t p = getParent (index);
        if (value->tile >= m_heap[p]->tile)
        {
            break;
        }

        // Priority queue is responsible for updating the index for each handle_t
        // if it is changed inside of it's vector. That way when the hash table returns
        // a pointer to it, it will always have the right index into the array for
        // the priority queue to use.
        m_heap[index] = m_heap[p];
        m_heap[index]->index = index;
        index = p;
    }
    m_heap[index] = value;
    m_heap[index]->index = index;
}

void PriorityQueue::makeHeap ()
{
    for (int i = (m_heap.size () - 1) / 2; i >= 0; --i)
    {
        downHeap (i);
    }
}

} /* namespace pathFind */
