#version 330 core

in vec2 coordinate;
uniform mat4 projection;

void main(void) {	
	gl_Position =  projection * vec4(coordinate, 0, 1);
}
