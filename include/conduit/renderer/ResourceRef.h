#ifndef CNDT_RENDERER_RESOURCE_REF_H
#define CNDT_RENDERER_RESOURCE_REF_H

#include "conduit/defines.h"

#include <functional>

namespace cndt {

class Renderer;

// Ref counted renderer resource reference
template <typename GpuType>
class RendererResRef {
    friend class Renderer;
    
    // Reference counter type
    // TODO: Atomic counter support
    using Counter = u64;

public:
    // Public null ref constructor
    RendererResRef() : 
        m_ptr(nullptr),
        m_ref_count(nullptr),
        m_deffer_delete_f([](GpuType*){ })
    { }

    // If the ref counter reaches zero call the deffer delete function
    ~RendererResRef()
    {
        release();
    }

    // Copy constructor
    RendererResRef(const RendererResRef&);
    // Copy assignment
    RendererResRef& operator=(const RendererResRef&);

    // Move constructor
    RendererResRef(RendererResRef&&);
    // Move assignment
    RendererResRef& operator=(RendererResRef&&);

    // Arrow access operator
    GpuType* operator-> ()
    {    
        return m_ptr;
    }

private:
    // Private constructor for the rendering backend
    RendererResRef(
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
void RendererResRef<GpuType>::release()
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
RendererResRef<GpuType>::RendererResRef(const RendererResRef& obj) : 
    m_ptr(obj.m_ptr),
    m_ref_count(obj.m_ref_count)
{
    if (m_ref_count != nullptr) {
        (*this->m_ref_count) += 1;
    }
}

// Copy assignment
template <typename GpuType>
RendererResRef<GpuType>& RendererResRef<GpuType>::operator=(
    const RendererResRef& obj
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
RendererResRef<GpuType>::RendererResRef(RendererResRef&& obj) : 
    m_ptr(obj.m_ptr),
    m_ref_count(obj.m_ref_count)
{
    obj.m_ptr = nullptr;
    obj.m_ref_count = nullptr;
}

// Move assignment
template <typename GpuType>
RendererResRef<GpuType>& RendererResRef<GpuType>::operator=(
    RendererResRef&& obj
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
