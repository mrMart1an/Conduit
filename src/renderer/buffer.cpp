#include "conduit/renderer/buffer.h"

namespace cndt {

// Decrease the counter and call the delete callback if it reaches 0
void GpuBufferRef::release()
{
    if (m_ref_count != nullptr) {
        (*m_ref_count) -= 1;

        // Call the deffer delete function when the counter reach zero
        if (*m_ref_count == 0) {
            m_deffer_delete_f();
            delete m_ref_count;

            m_id = nullId;
            m_ref_count = nullptr;
        }
    }
}

// If the ref counter reaches zero call the deffer delete function
GpuBufferRef::~GpuBufferRef()
{
    release();
}

// Copy constructor
GpuBufferRef::GpuBufferRef(const GpuBufferRef& obj) : 
    m_id(obj.m_id),
    m_ref_count(obj.m_ref_count)
{
    if (m_ref_count != nullptr) {
        (*this->m_ref_count) += 1;
    }
}

// Copy assignment
GpuBufferRef& GpuBufferRef::operator=(const GpuBufferRef& obj)
{
    release();

    this->m_id = obj.m_id;
    this->m_ref_count = obj.m_ref_count;

    if (m_ref_count != nullptr) {
        (*this->m_ref_count) += 1;
    }

    return *this;
}

// Move constructor
GpuBufferRef::GpuBufferRef(GpuBufferRef&& obj) : 
    m_id(obj.m_id),
    m_ref_count(obj.m_ref_count)
{
    obj.m_id = nullId;
    obj.m_ref_count = nullptr;
}

// Move assignment
GpuBufferRef& GpuBufferRef::operator=(GpuBufferRef&& obj)
{
    this->m_id = obj.m_id;
    this->m_ref_count = obj.m_ref_count;

    obj.m_id = nullId;
    obj.m_ref_count = nullptr;

    return *this;
}

} // namespace cndt
