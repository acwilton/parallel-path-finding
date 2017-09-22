/*
 * PriorityQueue.cc
 */

#include "algorithms/tools/PriorityQueue.h"

namespace pathFind
{

PriorityQueue::PriorityQueue ()
{

}

PriorityQueue::PriorityQueue (const std::vector<std::shared_ptr<PathTile>>& unheapedElements)
{
    m_heap = unheapedElements;
    makeHeap ();
}

PriorityQueue::PriorityQueue (std::vector<std::shared_ptr<PathTile>>&& unheapedElements)
{
    m_heap = std::move (unheapedElements);
    makeHeap ();
}

PriorityQueue::~PriorityQueue ()
{
}

void PriorityQueue::push (const PathTile& element)
{
    m_heap.emplace_back(element);
    upHeap (m_heap.size() - 1);
}

void PriorityQueue::push (PathTile&& element)
{
    m_heap.emplace_back(std::move (element));
    upHeap (m_heap.size() - 1);
}

void PriorityQueue::pop ()
{
    if (m_heap.empty())
    {
        return;
    }

    m_heap[0] = m_heap.back ();
    m_heap.pop_back ();
    downHeap (0);
}

PathTile PriorityQueue::top () const
{
    return m_heap[0];
}

void PriorityQueue::changeBestCost (std::shared_ptr<PathTile> tile, uint bestCost)
{

}

size_t PriorityQueue::getLeftChild (size_t index) const
{
    return 2 * index + 1;
}

size_t PriorityQueue::getRightChild (size_t index) const
{
    return 2 * index + 2;
}

size_t PriorityQueue::getParent (size_t index) const
{
    return ((index - 1) / 2);
}

void PriorityQueue::downHeap (size_t index)
{
    size_t i = index;
    PathTile value = m_heap[i];
    size_t c;
    while ((c = getLeftChild (i)) < m_heap.size ())
    {
        if (c + 1 < m_heap.size () && m_heap[c + 1] < m_heap[c])
        {
            ++c;
        }

        if (m_heap[c] >= value)
        {
            break;
        }

        m_heap[i] = m_heap[c];
        i = c;
    }
    m_heap[i] = value;
}

void PriorityQueue::upHeap (size_t index)
{
    PathTile value = m_heap[index];
    while (index != 0)
    {
        size_t p = getParent (index);
        if (value >= m_heap[p])
        {
            break;
        }
        m_heap[index] = m_heap[p];
        index = p;
    }
    m_heap[index] = value;
}

void PriorityQueue::makeHeap ()
{
    for (int i = (m_heap.size () - 1) / 2; i >= 0; --i)
    {
        downHeap (i);
    }
}

} /* namespace pathFind */
