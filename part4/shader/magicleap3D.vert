#version 330 core

in vec3 coordinate;
uniform mat4 projection;

void main(void) {	
	gl_Position =  projection * vec4(coordinate, 1);
}
