
#pragma once

#include "platform_includes.h"

namespace rb {
    
    class Camera3D {
    public:
        Camera3D();
        Camera3D(glm::vec3 position, glm::vec3 lookAt);
        
        virtual void Update(float delta);
        
        void SetPosition(glm::vec3 position) { _position.x = position.x; _position.y = position.y; _position.z = position.z; }
        void SetLookAt(glm::vec3 vector);
		glm::mat4 GetViewMatrix();
		glm::vec3 GetPosition() { return _position; }
		
        void MoveTo(glm::vec3 vector);
        void MoveBy(glm::vec3 vector);
        
    private:
        glm::vec3 _position;
        glm::vec3 _lookAt;
    };
    
}
