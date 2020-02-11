#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "RBShader.hpp"

namespace rb {

std::string Shader::s_shaderPath = "shader";

Shader::Shader() {
	_programID = -1;
	_coordinateId = -1;
	_projectionId = -1;
	_colorId = -1;
}

GLuint Shader::GetAttributeLocation(std::string name) {
	return glGetAttribLocation(_programID, name.c_str());
}

GLuint Shader::GetUniformLocation(std::string name) {
	return glGetUniformLocation(_programID, name.c_str());
}

void Shader::AppyTransform(glm::mat4 projection, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	glm::mat4 mT = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 mS = glm::scale(scale);
	glm::mat4 m = projection * mT * mS;
	
	ApplyProjection(m);
}

void Shader::ApplyProjection(glm::mat4 projection) {
	glUniformMatrix4fv(_projectionId, 1, GL_FALSE, (GLfloat*)&projection[0][0]);
}

void Shader::ApplyColor(glm::vec3 color) {
#ifdef MACOS_OPENGL
	glUniform4f(_colorId, color[0], color[1], color[2], 1.0);
#else
	glUniform3f(_colorId, color[0], color[1], color[2]);
#endif
}

void Shader::ApplyAndDrawVertices(GLfloat* vertices, int count, bool lineMode) {
	glEnableVertexAttribArray(_coordinateId);
	glVertexAttribPointer(_coordinateId, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	
	if (lineMode) {
		glDrawArrays(GL_LINES, 0, count);
	}
	else {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, count);
	}
	
	glBindVertexArray(0);
}

void Shader::DrawVBO(GLint vbo, int count, bool lineMode) {
	glBindVertexArray(vbo);

	if (lineMode) {
		glDrawArrays(GL_LINES, 0, count);
	}
	else {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, count);
	}

	glBindVertexArray(0);
	glUseProgram(0);
}

void Shader::Use() {
	if (_programID == -1) {
		RB_LOG(Error, "Cant use this shader; no program id");
		return;
	}
	
	glUseProgram(_programID);
}

void Shader::Load(const char * vertex_file_path, const char * fragment_file_path) {
	std::string fullPath = s_shaderPath + "/" + vertex_file_path;

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(fullPath, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";

		while (getline(VertexShaderStream, Line)) {
			VertexShaderCode += "\n" + Line;
		}

		VertexShaderStream.close();
	}
	else {
		RB_LOG(Error, "Can't open vertex shader '%s'", vertex_file_path);
		return;
	}

	fullPath = s_shaderPath + "/" + fragment_file_path;
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fullPath, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string line = "";

		while (getline(FragmentShaderStream, line)) {
			FragmentShaderCode += "\n" + line;
		}

		FragmentShaderStream.close();
	}
	else {
		RB_LOG(Error, "Can't open fragment shader: '%s'", fragment_file_path);
		return;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
	
	// Compile Vertex Shader
	RB_LOG(Debug, "Compile vertex shader '%s'", vertex_file_path);

	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexID, 1, &VertexSourcePointer , NULL);
	glCompileShader(vertexID);

	// Check Vertex Shader
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);

		glGetShaderInfoLog(vertexID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		RB_LOG(Debug, "Error compile vertex shader: '%s'", &VertexShaderErrorMessage[0]);

		return;
	}

	// Compile Fragment Shader
	RB_LOG(Debug, "Compile fragment shader '%s", fragment_file_path);

	GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(fragmentID);

	// Check Fragment Shader
	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(fragmentID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);

		RB_LOG(Debug, "Error compile fragment shader: '%s'", &FragmentShaderErrorMessage[0]);

		return;
	}

	// Link the program
	RB_LOG(Debug, "Link program");

	_programID = glCreateProgram();
	glAttachShader(_programID, vertexID);
	glAttachShader(_programID, fragmentID);
	glLinkProgram(_programID);

	// Check the program
	glGetProgramiv(_programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(_programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);

		RB_LOG(Error, "Error link shader: '%s'", &ProgramErrorMessage[0]);
		return;
	}

	RB_LOG(Debug, "Program id is %d", _programID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	
	// Map variables
	RB_LOG(Debug, "Shader variable mapping");
	
	_colorId = glGetUniformLocation(_programID, "color");
	_projectionId = glGetUniformLocation(_programID, "projection");
	_coordinateId = glGetAttribLocation(_programID, "coordinate");

	RB_LOG(Debug, "- color id: %d", _colorId);
	RB_LOG(Debug, "- projection id: %d", _projectionId);
	RB_LOG(Debug, "- coordinate id: %d", _coordinateId);
}

}
