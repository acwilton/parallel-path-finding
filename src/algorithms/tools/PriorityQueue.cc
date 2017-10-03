/*
 * PriorityQueue.cc
 */

#include "algorithms/tools/PriorityQueue.h"

namespace pathFind
{

PriorityQueue::PriorityQueue ()
{
}

PriorityQueue::PriorityQueue(const World& world)
{
    m_heap.reserve(world.getNumOpenTiles ());
    for (const auto& t : world)
    {
        if (t.cost != static_cast<uchar>(0))
        {
            m_heap.emplace_back (std::make_shared<handle_t>(
                    std::make_shared<PathTile>(t), t.id));
            m_hashTable[t.id] = m_heap.back ();
        }
    }
}

PriorityQueue::PriorityQueue (const std::vector<PathTile>& unheapedElements)
{
    m_heap.reserve(unheapedElements.size());
    for (const auto& e : unheapedElements)
    {
        m_heap.emplace_back (std::make_shared<handle_t>(
                std::make_shared<PathTile>(e), e.getID()));
        m_hashTable[e.getID ()] = m_heap.back ();
    }
    makeHeap ();
}

PriorityQueue::PriorityQueue (std::vector<std::shared_ptr<PathTile>>&& unheapedElements)
{
    m_heap.reserve(unheapedElements.size());
    for (const auto& e : unheapedElements)
    {
        m_heap.emplace_back (std::make_shared<handle_t>(
                e, e->getID()));
        m_hashTable[e->getID ()] = m_heap.back ();
    }
    makeHeap ();
}

PriorityQueue::~PriorityQueue ()
{
}

void PriorityQueue::push (const PathTile& element)
{
    std::shared_ptr<handle_t> newHandle = std::make_shared<handle_t> (
            std::make_shared<PathTile> (element), m_heap.size());
    m_hashTable[element.getID()] = newHandle;
    m_heap.emplace_back(newHandle);
    upHeap (m_heap.size() - 1);
}

void PriorityQueue::push (std::shared_ptr<PathTile>& element)
{
    std::shared_ptr<handle_t> newHandle = std::make_shared<handle_t> (
            element, m_heap.size());
    m_hashTable[element->getID()] = newHandle;
    m_heap.emplace_back(newHandle);
    upHeap (m_heap.size() - 1);
}

void PriorityQueue::pop ()
{
    if (m_heap.empty())
    {
        return;
    }

    m_hashTable.erase (m_heap[0]->tile().getID());
    m_heap[0] = m_heap.back ();
    m_heap.pop_back ();
    downHeap (0);
}

PathTile PriorityQueue::top () const
{
    return m_heap[0]->tile();
}

void PriorityQueue::changeBestCost (std::shared_ptr<PathTile> tile, uint bestCost)
{
    changeBestCost (tile->getID(), bestCost);
}

void PriorityQueue::changeBestCost (uint id, uint bestCost)
{
    m_hashTable[id]->tile().setBestCost (bestCost);
}

std::shared_ptr<PathTile> PriorityQueue::getPathTile (uint id) const
{
    auto ptr = m_hashTable.find (id);
    std::shared_ptr<PathTile> tile = (ptr == m_hashTable.end ()) ?
            nullptr :
            ptr->second->tilePtr;
    return tile;
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
        if (c + 1 < m_heap.size () && m_heap[c + 1]->tile() < m_heap[c]->tile())
        {
            ++c;
        }

        if (m_heap[c]->tile() >= value->tile())
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
        if (value->tile() >= m_heap[p]->tile())
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
