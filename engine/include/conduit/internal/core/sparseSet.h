#ifndef CNDT_CORE_SPARSE_SET_H
#define CNDT_CORE_SPARSE_SET_H

#include "conduit/defines.h"
#include "conduit/assert.h"

#include <iterator>
#include <utility>
#include <vector>

namespace cndt::internal {

template <typename Index, typename Value>
class SparseSet {
public:
    // Spare set iterator
    struct Iterator 
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::pair<Index, Value>;
        using pointer           = std::pair<Index*, Value*>;
        using reference         = std::pair<Index&, Value&>;

        Iterator(pointer ptr) : m_ptr(ptr) { }

        reference operator*() const { 
            reference ref { *m_ptr.first, *m_ptr.second };

            return ref;
        }
        pointer operator->() {
            return m_ptr;
        }
    
        // Prefix increment
        Iterator& operator++() { 
            m_ptr.first++;
            m_ptr.second++;

            return *this;
        }  
        // Postfix increment
        Iterator operator++(int) { 
            Iterator tmp = *this;
            ++(*this);

            return tmp;
        }
     
        // Prefix decrement
        Iterator& operator--() { 
            m_ptr.first--;
            m_ptr.second--;

            return *this;
        }  
        // Postfix decrement
        Iterator operator--(int) { 
            Iterator tmp = *this;
            --(*this);

            return tmp;
        }

        friend bool operator== (const Iterator& a, const Iterator& b) {
            return
                a.m_ptr.first == b.m_ptr.first &&
                a.m_ptr.second == b.m_ptr.second;
        };
        friend bool operator!= (const Iterator& a, const Iterator& b) {
            return
                !(a.m_ptr.first == b.m_ptr.first &&
                a.m_ptr.second == b.m_ptr.second);
        };

    private:
        pointer m_ptr;
    };

public:
    // Construct a spare set.
    // The given capacity is the starting size of the dense set
    // The max index size is the starting size of the sparse set
    SparseSet(usize capacity = 0, Index index_size = 0);

    // Access operator
    Value& operator [](Index idx); 

    // Erase an element from the sparse set
    void erase(Index idx);

    // Return a iterator to the beginning of the set
    Iterator begin();

    // Return a iterator to the beginning of the set
    Iterator end();

private:
    // Insert an element in the sparse set.
    // If the element already exist does nothing
    void insert(Index idx);

    // Search for an element in the spare set.
    // Return the index of the element in the dense storage if the
    // element exist, or return -1 if it doesn't
    usize search(Index idx);

    // Resize the sparse index storage
    void resizeSparse(usize size);

private:
    // Index sparse storage
    std::vector<usize> m_sparse_index;
    // Index dense storage
    std::vector<Index> m_dense_index;

    // Dense value storage
    std::vector<Value> m_dense_value;

    // Number of element in the dense storage
    usize m_elements_count;
};

// Construct a spare set.
// The given capacity is the starting size of the dense set
// The max index size is the starting size of the sparse set
template <typename Index, typename Value>
SparseSet<Index, Value>::SparseSet(usize capacity, Index index_size) :
    m_elements_count(0)
{
    // If the capacity isn't 0 set the spare set size to the
    // given size or capacity if index_size is 0
    if (capacity != 0) {
        if (index_size == 0) 
            resizeSparse(capacity);
        else
            resizeSparse(index_size);

        // Set the dense storage capacity
        m_dense_index.reserve(capacity);
        m_dense_value.reserve(capacity);
    }
}

// Access operator
template <typename Index, typename Value>
Value& SparseSet<Index, Value>::operator [](Index idx) 
{
    // Attempt to create a new element at the given index
    // if the element already exist does nothing
    insert(idx);

    // Return a reference to the dense storage value
    CNDT_DEBUG_ASSERT(search(idx) < m_elements_count)

    return m_dense_value[search(idx)];
}

// Erase an element from the sparse set
template <typename Index, typename Value>
void SparseSet<Index, Value>::erase(Index idx)
{
    usize dense_idx = search(idx);

    // Do nothing if the element isn't in the set
    if (dense_idx == -1)
        return;

    // Move the last element of the dense array 
    // to the deleted element slot
    m_dense_value[dense_idx] = m_dense_value.back();
    m_dense_index[dense_idx] = m_dense_index.back();

    // Update the moved element index in the sparse array
    usize moved_idx = m_dense_index.back();
    m_sparse_index[moved_idx] = dense_idx;

    // Remove the last element from dense storage
    m_dense_value.pop_back();
    m_dense_index.pop_back();

    // Set the sparse index to -1
    m_sparse_index[idx] = -1;

    // Decrement element count
    m_elements_count--;
}

// Return a iterator to the beginning of the set
template <typename Index, typename Value>
SparseSet<Index, Value>::Iterator SparseSet<Index, Value>::begin() 
{ 
    return Iterator(
        std::pair<Index*, Value*>(
            m_dense_index.data(),
            m_dense_value.data()
        )
    );
}

// Return a iterator to the beginning of the set
template <typename Index, typename Value>
SparseSet<Index, Value>::Iterator SparseSet<Index, Value>::end() 
{ 
    return Iterator(
        std::pair<Index*, Value*>(
            &m_dense_index.data()[m_elements_count],
            &m_dense_value.data()[m_elements_count]
        )
    );
}

// Insert an element in the sparse set.
// If the element already exist does nothing
template <typename Index, typename Value>
void SparseSet<Index, Value>::insert(Index idx)
{
    // Check if the element already exist
    if (search(idx) != -1)
        return;

    // Check if the given index is greater than the sparse index storage
    // and expand it if it is
    if (m_sparse_index.size() <= idx)
        resizeSparse(idx + 1);

    // Get the dense storage index
    usize dense_index = m_elements_count;

    // Store the dense index in the sparse array
    // and store the actual index in the dense sparse array
    m_sparse_index[idx] = dense_index;
    m_dense_index.push_back(idx);

    // Construct a value and place it in the dense storage
    m_dense_value.emplace_back();

    // Increment the elements count
    m_elements_count++;
}

// Search for an element in the spare set.
// Return the index of the element in the dense storage if the
// element exist, or return -1 if it doesn't
template <typename Index, typename Value>
usize SparseSet<Index, Value>::search(Index idx)
{
    // If the spare index storage is smaller than index, 
    // the element isn't in the spare set
    if (m_sparse_index.size() <= idx)
        return -1;

    // Return the index of the item in the dense array
    return m_sparse_index[idx];
}

// Resize the sparse index storage
template <typename Index, typename Value>
void SparseSet<Index, Value>::resizeSparse(usize size)
{
    CNDT_DEBUG_ASSERT(size > m_elements_count)

    // Create a new sparse set array and initialize it with -1
    m_sparse_index = std::vector<usize>(size, -1);

    // Go over the dense index array and reinitialize the spare storage 
    for (usize i = 0; i < m_elements_count; i++) {
        Index idx = m_dense_index[i]; 

        // Store the dense array index in the sparse array
        CNDT_DEBUG_ASSERT(idx < m_sparse_index.size())
        m_sparse_index[idx] = i;
    }
}

} // namespace cndt

#endif
