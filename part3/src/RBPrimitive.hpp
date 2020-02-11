
#pragma once

#include "platform_includes.h"
#include "RBActor.hpp"

namespace rb {
   
class Primitive : public Actor {

// Types
public:
	typedef enum {
		Cube = 0,
		Cylinder,
		Cone,
		Pyramide
	} Shape3D;

public:
	Primitive(Shape3D shape = Cube);
	
	virtual void Update(float delta);
	virtual void Render(Shader& shader, glm::mat4 projectionMatrix);
	virtual void RenderDirect(Shader& shader, glm::mat4 projectionMatrix);

	virtual bool TestCollisionAABB(Actor* actor);

	void RenderTest(Shader& shader, glm::mat4 projectionMatrix);
	void Dump();

private:
	virtual void RenderInternal(Shader& shader, glm::mat4 projectionMatrix);
	GLuint _CreateVAO(Shader shader, GLfloat* vertices, int size);
	void _CreateAllVAOs(Shader shader);

// Transform related
public:
	void SetTransform(glm::mat4 transform) { _transform = transform; }

	void SetSpeed(float x, float y, float z) { _speed.x = x; _speed.y = y; _speed.z = z; }
	void SetSpeed(glm::vec3 speed) { _speed = speed; }
	glm::vec3 GetSpeed() { return _speed; }
	
	void SetRotationSpeed(float x, float y, float z) { _rotationSpeed.x = x; _rotationSpeed.y = y; _rotationSpeed.z = z; }
	void SetRotationSpeed(glm::vec3 speed) { _rotationSpeed = speed; }
	glm::vec3 GetRotationSpeed() { return _rotationSpeed; }
	
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetPosition(glm::vec3 position) { _position = position; }
	glm::vec3 GetPosition() { return _position; }
	
	void RotateTo(glm::vec3 vector);
	glm::vec3 GetRotation() { return _rotation; }

	void ScaleTo(float scale);
	glm::vec3 GetScale() { return _scale; }

	glm::vec3 GetSize();
	
// Material related
public:
	void SetColor(float r, float g, float b, float a) { _color.r = r; _color.g = g; _color.b = b; }
	void SetColor(glm::vec3 color) { _color.r = color.r; _color.g = color.g; _color.b = color.b; }
	glm::vec3 GetColor() { return _color; }

// Transform related
private:
	static GLuint _cubeVaoID;
	static GLuint _cylinderVaoID;
	static GLuint _coneVaoID;
	static GLuint _pyramidVaoID;

	glm::vec3 _position;
	glm::vec3 _rotation;    // Stored as degrees
	glm::vec3 _scale;
	glm::vec3 _speed;
	glm::vec3 _rotationSpeed;

	glm::mat4 _transform;	// Can set directly but overrides then the transform related stuff
// Material/Mesh related
private:
	glm::vec3 _color;
	Shape3D _shape;
};
    
}

