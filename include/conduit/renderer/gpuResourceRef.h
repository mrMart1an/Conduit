#ifndef CNDT_GPU_RESOURCE_REF_H
#define CNDT_GPU_RESOURCE_REF_H

#include "conduit/defines.h"

#include <functional>

namespace cndt {

class Renderer;

// Ref counted GPU resource reference
template <typename GpuType>
class GpuResourceRef {
    friend class Renderer;
    
    // Reference counter type
    // TODO: Atomic counter support
    using Counter = u64;

public:
    // Public null ref constructor
    GpuResourceRef() : 
        m_ptr(nullptr),
        m_ref_count(nullptr),
        m_deffer_delete_f([](GpuType*){ })
    { }

    // If the ref counter reaches zero call the deffer delete function
    ~GpuResourceRef()
    {
        release();
    }

    // Copy constructor
    GpuResourceRef(const GpuResourceRef&);
    // Copy assignment
    GpuResourceRef& operator=(const GpuResourceRef&);

    // Move constructor
    GpuResourceRef(GpuResourceRef&&);
    // Move assignment
    GpuResourceRef& operator=(GpuResourceRef&&);

    // Arrow access operator
    GpuType* operator-> ()
    {    
        return m_ptr;
    }

private:
    // Private constructor for the rendering backend
    GpuResourceRef(
        GpuType* ptr,
        std::function<void(GpuType*)> deffer_delete_f
    ) : 
        m_ptr(ptr),
        m_ref_count(new Counter),
        m_deffer_delete_f(deffer_delete_f)
    { }

    // Decrease the counter and call the delete callback if it reaches 0
    void release();

private:
    // Buffer id used by the rendering backend to identify the buffer
    GpuType* m_ptr;

    // Count the number of existing buffer reference
    Counter* m_ref_count;

    // Deffer delete function callback
    std::function<void(GpuType*)> m_deffer_delete_f;
};

// Decrease the counter and call the delete callback if it reaches 0
template <typename GpuType>
void GpuResourceRef<GpuType>::release()
{
    if (m_ref_count != nullptr) {
        (*m_ref_count) -= 1;

        // Call the deffer delete function when the counter reach zero
        if (*m_ref_count == 0) {
            m_deffer_delete_f(m_ptr);
            delete m_ref_count;

            m_ptr = nullptr;
            m_ref_count = nullptr;
        }
    }
}

// Copy constructor
template <typename GpuType>
GpuResourceRef<GpuType>::GpuResourceRef(const GpuResourceRef& obj) : 
    m_ptr(obj.m_ptr),
    m_ref_count(obj.m_ref_count)
{
    if (m_ref_count != nullptr) {
        (*this->m_ref_count) += 1;
    }
}

// Copy assignment
template <typename GpuType>
GpuResourceRef<GpuType>& GpuResourceRef<GpuType>::operator=(
    const GpuResourceRef& obj
) {
    release();

    this->m_ptr = obj.m_ptr;
    this->m_ref_count = obj.m_ref_count;

    if (m_ref_count != nullptr) {
        (*this->m_ref_count) += 1;
    }

    return *this;
}

// Move constructor
template <typename GpuType>
GpuResourceRef<GpuType>::GpuResourceRef(GpuResourceRef&& obj) : 
    m_ptr(obj.m_ptr),
    m_ref_count(obj.m_ref_count)
{
    obj.m_ptr = nullptr;
    obj.m_ref_count = nullptr;
}

// Move assignment
template <typename GpuType>
GpuResourceRef<GpuType>& GpuResourceRef<GpuType>::operator=(
    GpuResourceRef&& obj
) {
    release();

    this->m_ptr = obj.m_ptr;
    this->m_ref_count = obj.m_ref_count;

    obj.m_ptr = nullptr;
    obj.m_ref_count = nullptr;

    return *this;
}


} // namespace cndt

#endif
