#ifndef CNDT_RENDERER_GRAPH_RESOURCES_H
#define CNDT_RENDERER_GRAPH_RESOURCES_H

#include "conduit/defines.h"

namespace cndt {

// Render graph resource handle
class GraphResource {
public:
    // Resource id type
    using Id = u64; 
    // Null resource id
    static constexpr Id nullId = UINT64_MAX;

    // Resource lifetime type
    enum class LifetimeType {
        // Undefined lifetime
        Undefined,

        // Externally manage lifetime
        External,
        // Graph lifetime lifetime
        Transient
    };

    // Resource type 
    enum class ResourceType {
        // Undefined type
        Undefined,

        // Buffer type
        Buffer,
        // Image type
        Image
    };

public:
    GraphResource() : 
        m_id(nullId),
        m_lifetime(LifetimeType::Undefined),
        m_type(ResourceType::Undefined)
    { }
    GraphResource(ResourceType type) : 
        m_id(nullId),
        m_lifetime(LifetimeType::Undefined),
        m_type(type)
    { }
    GraphResource(
        Id id,
        LifetimeType lifetime,

        ResourceType type
    ) : 
        m_id(id),
        m_lifetime(lifetime), 
        m_type(type)
    { }

    // Id getter
    Id id() const { return m_id; }
    // Resource lifetime type getter
    LifetimeType lifetime() const { return m_lifetime; }
    
    // Resource type getter
    ResourceType type() const { return m_type; }

private:
    // Store resource Id
    Id m_id;
    // Resource lifetime type
    LifetimeType m_lifetime;

    // Resource type
    ResourceType m_type;
};

// Buffer resource handle type
class GraphBuffer : public GraphResource {
public:
    GraphBuffer() : GraphResource(ResourceType::Buffer) { }
    GraphBuffer(Id id, LifetimeType lifetime) : GraphResource(
        id, lifetime,
        ResourceType::Buffer
    ) { }
};

// Image resource handle type
class GraphImage : public GraphResource {
public:
    GraphImage() : GraphResource(ResourceType::Buffer) { }
    GraphImage(Id id, LifetimeType lifetime) : GraphResource(
        id, lifetime,
        ResourceType::Image
    ) { }
};

} // namespace cndt

#endif
