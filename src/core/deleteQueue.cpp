#include "conduit/internal/core/deleteQueue.h"

namespace cndt {

void DeleteQueue::addDeleter(std::function<void(void)> deleter_fun) 
{
    m_deleters.push_back(deleter_fun);
}

void DeleteQueue::callDeleter()
{
    // Call the deleter functions in reverse order
    // reverse iterate the deletion queue to execute all the functions
    while (!m_deleters.empty()) {
        // Call the function and then remove it from the queue
        m_deleters.back()();
        m_deleters.pop_back();
    }
}

// Check if the queue is empty
bool DeleteQueue::empty()
{
    return m_deleters.empty();
}

} // namespace cndt::vulkan
