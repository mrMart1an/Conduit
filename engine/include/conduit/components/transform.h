#ifndef CNDT_COMPONENT_TRANSFORM_H
#define CNDT_COMPONENT_TRANSFORM_H

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cndt {

// Conduit transform component
class Transform {
public:
    Transform() : 
        m_position(0, 0, 0),
        m_rotation(1, 0, 0, 0),
        m_scale(1, 1, 1)
    { };

    // Return a reference to the object position 
    // vector relative to the parent object
    glm::vec3& position() { return m_position; }
    
    // Return a reference to the object rotation 
    // quaternion relative to the parent object
    glm::quat& rotation() { return m_rotation; }

    // Return a reference to the object scale
    glm::vec3& scale() { return m_scale; }

    // Return to the object rotation relative to 
    // the parent object as an euler angles vector
    // The angle are expressed in radians 
    glm::vec3 getRotationEuler() const { return glm::eulerAngles(m_rotation); }

    // Set the object rotation relative to the parent object from 
    // the given vector of euler angles expressed in radians
    void setRotationEuler(glm::vec3 euler) { m_rotation = glm::quat(euler); }

    // Return the translation matrix of the object relative to the parent
    glm::mat4 getTranslationMat4() const 
    { 
        return glm::translate(glm::mat4(1.0f), m_position); 
    }
    
    // Get the rotation matrix of the object relative to the parent
    glm::mat4 getRotationMat4() const { return glm::mat4_cast(m_rotation); }
    
    // Get the scale matrix of the object relative to the parent
    glm::mat4 getScaleMat4() const 
    { 
        return glm::scale(glm::mat4(1.0f), m_scale); 
    }

    // Return a model matrix relative to the parent object
    glm::mat4 getModelMat4() const 
    {
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_position);
        glm::mat4 rotate = glm::mat4_cast(m_rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

        return translate * rotate * scale;
    }
    
protected:
    // Store the object position relative to the parent object
    glm::vec3 m_position;
    // Store the object rotation relative to the parent object
    glm::quat m_rotation;
    // Store the object scale
    glm::vec3 m_scale;
};

// Global Object transform in world space
class GlobalTransform : public Transform 
{ 
    GlobalTransform() : Transform() { };
};


} // namespace cndt

#endif
