#ifndef CNDT_DELETE_QUEUE
#define CNDT_DELETE_QUEUE

#include <functional>
#include <vector>

namespace cndt {

// Call the deleter functions in the opposite order to hot they were added
class DeleteQueue {
public:
    DeleteQueue() = default;
    ~DeleteQueue() = default;

    // Add a deleter to the deleter queue
    void addDeleter(std::function<void(void)> deleter_fun);

    // Call all of the deleter functions in the queue
    void callDeleter();

    // Check if the queue is empty
    bool empty();

private:
    // Store the deleter functions
    std::vector<std::function<void(void)>> m_deleters;
};

} // namespace cndt

#endif
