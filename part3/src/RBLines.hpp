
#pragma once

#include "platform_includes.h"
#include "RBActor.hpp"
#include <vector>

#define MAX_VERTICES 1000
#define MAX_SIZE MAX_VERTICES * 3

namespace rb {
   
class Lines : public Actor {

public:
	Lines();
	
	virtual void Update(float delta);
	virtual void Render(Shader& shader, glm::mat4 projectionMatrix);
	void RenderTest(Shader& shader, glm::mat4 projectionMatrix);
	void Dump();

// Lines related
public:
	void AddLine(glm::vec3 from, glm::vec3 to);
	void Clear();
	
private:
	GLuint _CreateVAO(Shader shader, GLfloat* vertices, int size);

// Transform related
public:
	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }
	void SetPosition(glm::vec3 position) { _position = position; }
	glm::vec3 GetPosition() { return _position; }
	void SetScale(float scale) { _scale = scale; }
	
// Material related
public:
	void SetColor(float r, float g, float b, float a) { _color.r = r; _color.g = g; _color.b = b; }
	void SetColor(glm::vec3 color) { _color.r = color.r; _color.g = color.g; _color.b = color.b; }
	glm::vec3 GetColor() { return _color; }

// Transform related
private:
	glm::vec3 _position;

// Material/Mesh related
private:
	GLuint _vaoID;
	glm::vec3 _color;
	GLfloat _vertices[MAX_SIZE];
	int _pointCount;
	float _scale;
};
    
}

