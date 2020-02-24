#pragma once

#include "platform_includes.h"

#include <string>

namespace rb {

class Shader {
public:
	Shader();
	~Shader() {}

	GLuint GetProgramID() { return _programID; }
	GLuint GetAttributeLocation(std::string name);
	GLuint GetUniformLocation(std::string name);
	
	void AppyTransform(glm::mat4 projection, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	void ApplyProjection(glm::mat4 projection);
	void ApplyColor(glm::vec3 color);
	void ApplyAndDrawVertices(GLfloat* vertices, int count, bool lineMode = false);
	void DrawVBO(GLint vbo, int count, bool lineMode = false);
	
	void Load(const char *vertex_file_path, const char *fragment_file_path);
	void Use();
	
public:
	static void SetShaderPath(std::string path) { s_shaderPath = path; }

private:
	GLuint _programID;
	GLuint _coordinateId;
	GLuint _projectionId;
	GLuint _colorId;

	static std::string s_shaderPath;
};

}
