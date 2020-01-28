#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <ml_logging.h>

#include "RBShader.h"

GLuint Shader::GetAttributeLocation(std::string name) {
	return glGetAttribLocation(_programID, name.c_str());
}

GLuint Shader::GetUniformLocation(std::string name) {
	return glGetUniformLocation(_programID, name.c_str());
}

void Shader::Load(const char * vertex_file_path, const char * fragment_file_path) {
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";

		while (getline(VertexShaderStream, Line)) {
			VertexShaderCode += "\n" + Line;
		}

		VertexShaderStream.close();
	}
	else {
		ML_LOG(Error, "Can't open vertex shader '%s'", vertex_file_path);
		return;
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";

		while (getline(FragmentShaderStream, Line)) {
			FragmentShaderCode += "\n" + Line;
		}

		FragmentShaderStream.close();
	}
	else {
		ML_LOG(Error, "Can't open fragment shader: '%s'", fragment_file_path);
		return;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);

	// Compile Vertex Shader
	ML_LOG(Debug, "Compile vertex shader '%s", vertex_file_path);

	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexID, 1, &VertexSourcePointer , NULL);
	glCompileShader(vertexID);

	// Check Vertex Shader
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);

		glGetShaderInfoLog(vertexID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		ML_LOG(Debug, "Error compile vertex shader: '%s'", &VertexShaderErrorMessage[0]);

		return;
	}

	// Compile Fragment Shader
	ML_LOG(Debug, "Compile fragment shader '%s", fragment_file_path);

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

		ML_LOG(Debug, "Error compile fragment shader: '%s'", &FragmentShaderErrorMessage[0]);

		return;
	}

	// Link the program
	ML_LOG(Debug, "Link program");

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

		ML_LOG(Error, "Error link shader: '%s'", &ProgramErrorMessage[0]);
	}

	ML_LOG(Debug, "Program id is %d", _programID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}
