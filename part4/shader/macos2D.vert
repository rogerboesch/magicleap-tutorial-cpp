uniform vec2 coordinate;
uniform mat4 projection;
uniform vec4 color;

void main(void) {	
	gl_Position =  projection * vec4(coordinate, 0, 1);
}
