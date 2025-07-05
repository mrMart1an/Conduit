#ifndef CNDT_ECS_SPARE_SET_STORAGE_H
#define CNDT_ECS_SPARE_SET_STORAGE_H

#include "conduit/defines.h"
#include "conduit/assert.h"
#include "conduit/ecs/entity.h"

#include "conduit/internal/ecs/componentStorage.h"

#include <vector>

namespace cndt::ecs::internal {

// Spare set storage for components
template <typename T>
class SparseSetStorage {

public:
    // Insert an element in the sparse set.
    // If the element already exist does nothing
    void insert(Entity entity, Tick add_tick, T& comp);
    // Insert an element in the sparse set.
    // If the element already exist does nothing
    template <typename... Args>
    void insertEmplace(Entity entity, Tick add_tick, Args& ...args);

    // Erase an element from the sparse set
    void erase(Entity idx);

private:
    // Search for an element in the spare set.
    // Return the index of the element in the dense storage if the
    // element exist, or return -1 if it doesn't
    usize search(Entity entity);

    // Resize the sparse index storage
    void resizeSparse(usize size);

private:
    // Store the index in the dense array at any given entity Id
    // Used for fast component look up
    std::vector<usize> m_sparse_index;

    // Store the components as a dense list
    ComponentStorage<T> m_dense_storage;
    // Store the entity at the corresponding component slot 
    // used during iteration to determine what entity owns the component 
    std::vector<Entity> m_dense_entity;

    // Number of element in the dense storage
    usize m_elements_count;
};

// Insert an element in the sparse set.
// If the element already exist does nothing
template <typename T>
void SparseSetStorage<T>::insert(Entity entity, Tick add_tick, T& comp)
{
    // Check if the element already exist
    if (search(entity) != -1)
        return;

    // Check if the given index is greater than the sparse index storage
    // and expand it if it is
    if (m_sparse_index.size() <= entity.index())
        resizeSparse(entity.index() + 1);

    // Get the dense storage index
    usize dense_index = m_elements_count;

    // Store the dense index in the sparse array
    // and store the actual index in the dense sparse array
    m_sparse_index[entity.index()] = dense_index;
    m_dense_entity.push_back(entity);

    // Construct a value and place it in the dense storage
    m_dense_storage.pushBack(add_tick, comp);

    // Increment the elements count
    m_elements_count++;
}

// Insert an element in the sparse set.
// If the element already exist does nothing
template <typename T>
    template <typename... Args>
void SparseSetStorage<T>::insertEmplace(
    Entity entity, 
    Tick add_tick,
    Args& ...args
) {
    // Check if the element already exist
    if (search(entity) != -1)
        return;

    // Check if the given index is greater than the sparse index storage
    // and expand it if it is
    if (m_sparse_index.size() <= entity.index())
        resizeSparse(entity.index() + 1);

    // Get the dense storage index
    usize dense_index = m_elements_count;

    // Store the dense index in the sparse array
    // and store the actual index in the dense sparse array
    m_sparse_index[entity.index()] = dense_index;
    m_dense_entity.push_back(entity);

    // Construct a value and place it in the dense storage
    m_dense_storage.emplaceBack(add_tick, args...);

    // Increment the elements count
    m_elements_count++;
}

// Search for an element in the spare set.
// Return the index of the element in the dense storage if the
// element exist, or return -1 if it doesn't
template <typename T>
usize SparseSetStorage<T>::search(Entity entity)
{
    // If the spare index storage is smaller than index, 
    // the element isn't in the spare set
    if (m_sparse_index.size() <= entity.index())
        return -1;

    // Return the index of the item in the dense array
    return m_sparse_index[entity.index()];
}

// Resize the sparse index storage
template <typename T>
void SparseSetStorage<T>::resizeSparse(usize size)
{
    CNDT_DEBUG_ASSERT(size > m_elements_count)

    // Create a new sparse set array and initialize it with -1
    m_sparse_index = std::vector<usize>(size, -1);

    // Go over the dense index array and reinitialize the spare storage 
    for (usize i = 0; i < m_elements_count; i++) {
        Entity entity = m_dense_entity[i]; 

        // Store the dense array index in the sparse array
        CNDT_DEBUG_ASSERT(entity.index() < m_sparse_index.size())
        m_sparse_index[entity.index()] = i;
    }
}

// Erase an element from the sparse set
template <typename T>
void SparseSetStorage<T>::erase(Entity entity)
{
    usize dense_idx = search(entity.index());

    // Do nothing if the element isn't in the set
    if (dense_idx == -1)
        return;

    // Move the last element of the dense array 
    // to the deleted element slot
    m_dense_storage[dense_idx] = m_dense_storage.back();
    m_dense_entity[dense_idx] = m_dense_entity.back();

    // Update the moved element index in the sparse array
    usize moved_idx = m_dense_entity.back().index();
    m_sparse_index[moved_idx] = dense_idx;

    // Remove the last element from dense storage
    m_dense_storage.pop_back();
    m_dense_entity.pop_back();

    // Set the sparse index to -1
    m_sparse_index[entity.index()] = -1;

    // Decrement element count
    m_elements_count--;
}
} // namespace cndt::ecs::internal

#endif
