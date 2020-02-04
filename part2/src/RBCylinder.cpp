#include "RBCylinder.h"
#include "RBShader.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#include "../glm/gtc/matrix_transform.hpp"

const char APP_TAG[] = "C-ENGINE-CYL";
#include <ml_logging.h>

#define BOTTOM_RADIUS 0.5
#define TOP_RADIUS 0.5

static void MakeCylinder(GLfloat *vertices, int numSteps) {
	float step = 2 * M_PI / static_cast<float>(numSteps);
	float a = 0;
	int idx = 0;

	for (int i = 0; i < numSteps + 1; i++) {
		float x = cosf(a);
		float z = sinf(a);

		vertices[idx++] = x * BOTTOM_RADIUS;
		vertices[idx++] = 0;
		vertices[idx++] = z * BOTTOM_RADIUS;
		vertices[idx++] = x * TOP_RADIUS;
		vertices[idx++] = 1.0;
		vertices[idx++] = z * TOP_RADIUS;

		a += step;
	}
}

Cylinder::Cylinder(int steps) {
	_steps = steps;
	_verts = (_steps + 1) * 2; 

	_position = glm::vec3(0);
	_rotation = glm::vec3(0);
	_scale = glm::vec3(0.25);
}

Cylinder::~Cylinder() {
}

void Cylinder::ApplyShader(Shader& shader) {
	_progId = shader.GetProgramID();
	glUseProgram(_progId);

	_colorId = glGetUniformLocation(_progId, "color");
	_projId = glGetUniformLocation(_progId, "projFrom3D");
	GLuint location = glGetAttribLocation(_progId, "coord3D");

	glGenVertexArrays(1, &_vaoId);
	glBindVertexArray(_vaoId);

	GLfloat cylinderVertexData[_verts * 3];
	MakeCylinder(cylinderVertexData, _steps);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
	glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderVertexData), cylinderVertexData, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glUseProgram(0);

	ML_LOG_TAG(Debug, APP_TAG, "Uniform location (%d, %d, %d), program %d", _colorId, _projId, location, _progId);
}

void Cylinder::Render(glm::mat4 projectionMatrix) {
	glUseProgram(_progId);

	glm::mat4 translate = glm::translate(glm::mat4(1.0f), _position);
	glm::mat4 scale = glm::scale(_scale);
	glm::mat4 transform = projectionMatrix * translate * scale;

	glBindVertexArray(_vaoId);
	glUniformMatrix4fv(_projId, 1, GL_FALSE, &transform[0][0]);
	glUniform3f(_colorId, _color[0], _color[1], _color[2]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, _verts);
	glBindVertexArray(0);

	glUseProgram(0);
}

void Cylinder::Dump() {
	ML_LOG_TAG(Debug, APP_TAG, "---------------- Dump of cylinder ----------------");
	ML_LOG_TAG(Debug, APP_TAG, "Prog ID:  %d", _progId);
	ML_LOG_TAG(Debug, APP_TAG, "VAO ID:   %d", _vaoId);
	ML_LOG_TAG(Debug, APP_TAG, "Proj ID:  %d", _projId);
	ML_LOG_TAG(Debug, APP_TAG, "Color ID: %d", _colorId);
	ML_LOG_TAG(Debug, APP_TAG, "Steps:    %d",    _steps);
	ML_LOG_TAG(Debug, APP_TAG, "Verts:    %d",    _verts);
	ML_LOG_TAG(Debug, APP_TAG, "Color:    %f,%f,%f", _color[0], _color[1], _color[2]);
	ML_LOG_TAG(Debug, APP_TAG, "Position: %f,%f,%f", _position.x, _position.y, _position.z);
	ML_LOG_TAG(Debug, APP_TAG, "Rotation: %f,%f,%f", _rotation.x, _rotation.y, _rotation.z);
	ML_LOG_TAG(Debug, APP_TAG, "Scale:    %f,%f,%f", _scale.x, _scale.y, _scale.z);
	ML_LOG_TAG(Debug, APP_TAG, "--------------------------------------------------");
}