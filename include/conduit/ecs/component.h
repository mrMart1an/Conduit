#ifndef CNDT_ESC_COMPONENT_H
#define CNDT_ESC_COMPONENT_H

namespace cndt::ecs {

class Component {
protected:
    // Represent the storage technique used by the component buffer 
    // to store instance of this component
    //
    // - Linear:    cache friendly and fast access time 
    //              but slow add and remove operation
    // - Sparse:    non cache friendly and slower access time
    //              but fast add and removal operation
    enum class StorageType {
        Linear,
        Sparse
    };

protected:
    // Return the component buffer storage type use for this component
    // The default storage type is Linear
    StorageType componentStorageType() const 
    { 
        return StorageType::Linear;
    }
};

} // namespace cndt 

#endif
