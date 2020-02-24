
#include "RBCamera3D.hpp"

namespace rb {
    
    Camera3D::Camera3D() {
        _position = glm::vec3(0, 0, 0);
        _lookAt = glm::vec3(0, 0, 0);
        
        RB_LOG(Debug, "Camera at: 0,0,0 - look at: 0,0,0");
    }
    
    Camera3D::Camera3D(glm::vec3 position, glm::vec3 lookAt) : Camera3D() {
        _position = position;
        _lookAt = lookAt;
        
        RB_LOG(Debug, "Camera at: %.0f,%.0f,%.0f - look at %.0f, %.0f, %.0f", _position.x, _position.y, _position.z, _lookAt.x, _lookAt.y, _lookAt.z);
    }
    
    void Camera3D::Update(float delta) {
    }
    
    // -------------------------------------------------------------------------
    #pragma mark - Properties

	glm::mat4 Camera3D::GetViewMatrix() {
		return glm::lookAt(_position, _lookAt,  glm::vec3(0,1,0));
	}

    void Camera3D::SetLookAt(glm::vec3 vector) {
        _lookAt = vector;

        RB_LOG(Debug, "Camera at: %.0f,%.0f,%.0f - look at %.0f, %.0f, %.0f", _position.x, _position.y, _position.z, _lookAt.x, _lookAt.y, _lookAt.z);
    }
    
    // -------------------------------------------------------------------------
    #pragma mark - Move support
    
    void Camera3D::MoveBy(glm::vec3 vector) {
        _position.x += vector.x;
        _position.y += vector.y;
        _position.z += vector.z;
    }
    
    void Camera3D::MoveTo(glm::vec3 vector) {
        _position.x = vector.x;
        _position.y = vector.y;
        _position.z = vector.z;
    }
    
}
