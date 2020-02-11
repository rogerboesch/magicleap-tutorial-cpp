
#include "RBLines.hpp"

namespace rb {

// -----------------------------------------------------------------------------
#pragma mark - Lines support

Lines::Lines() : Actor() {
	_position = glm::vec3(0.0);
	_color = COLOR_RED;
	_vaoID = -1;
	_pointCount = 0;
	_scale = 1.0f;
	memset(&_vertices[0], 0, MAX_SIZE);
	
	RB_LOG(Debug, "Lines %lu added", GetID());
}

void Lines::Update(float delta) {
	Actor::Update(delta);
}

GLuint Lines::_CreateVAO(Shader shader, GLfloat* vertices, int size) {
	GLuint vao = -1;

	glGenVertexArrays(1, &vao);
	glUseProgram(shader.GetProgramID());
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	GLuint location = shader.GetAttributeLocation("coordinate");

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
	glUseProgram(0);

	RB_LOG(Debug, "Create vertex array buffer (VBO) for location '%d' in lines '%lu' (%d points)", vao, GetID(), _pointCount);

	return vao;
}

void Lines::Render(Shader& shader, glm::mat4 projectionMatrix) {
	if (IsHidden()) return;

	if (_pointCount < 6) {
		// Minimum of 2 points needed
		return;
	}
	
	shader.Use();

	shader.ApplyColor(_color);

	glm::mat4 mT = glm::translate(glm::mat4(1.0f), _position);
	glm::mat4 m = projectionMatrix * mT;
	shader.ApplyProjection(m);

	shader.ApplyAndDrawVertices(&_vertices[0], _pointCount/3, true);
}

void Lines::RenderTest(Shader& shader, glm::mat4 projectionMatrix) {
	if (_vaoID == -1) {
		_vaoID = _CreateVAO(shader, (GLfloat*)&_vertices[0], MAX_SIZE);
	}

	shader.Use();

	shader.ApplyColor(_color);

	glm::mat4 mT = glm::translate(glm::mat4(1.0f), _position);
	glm::mat4 mS = glm::scale(glm::vec3(_scale));
	glm::mat4 m = projectionMatrix * mT * mS;
	shader.ApplyProjection(m);
	shader.DrawVBO(_vaoID, _pointCount/3, true);
}

void Lines::AddLine(glm::vec3 from, glm::vec3 to) {
	if (_pointCount*3 + 6 > MAX_VERTICES) {
		RB_LOG(Error, "Line buffer reaches limit of %d vertices", MAX_VERTICES);
		return;
	}

	_vertices[_pointCount++] = from.x;
	_vertices[_pointCount++] = from.y;
	_vertices[_pointCount++] = from.z;
	_vertices[_pointCount++] = to.x;
	_vertices[_pointCount++] = to.y;
	_vertices[_pointCount++] = to.z;
}

void Lines::Clear() {
	// TODO: Implement buffer update
	_pointCount = 0;
	memset(&_vertices[0], 0, MAX_SIZE);
}

void Lines::Dump() {
	RB_LOG(Debug, "-------------- Dump of line #%03d ----------------", (int)GetID());
	RB_LOG(Debug, "GLUid ID: %d", _vaoID);
	RB_LOG(Debug, "     Tag: %d", GetTag());
	RB_LOG(Debug, "  Hidden: %d", IsHidden());
	RB_LOG(Debug, "   Color: %f,%f,%f", _color[0], _color[1], _color[2]);
	RB_LOG(Debug, "Position: %f,%f,%f", _position.x, _position.y, _position.z);
	RB_LOG(Debug, "   Scale: %f", _scale);
	RB_LOG(Debug, "------------------ Vertices: %d ------------------", _pointCount/3);
	
	for (int i = 0; i < _pointCount; i++) {
		RB_LOG(Debug, "%d: %f, %f, %f", i/3, _vertices[i], _vertices[i+1], _vertices[i+2]);
		i += 2;
	}

	RB_LOG(Debug, "--------------------------------------------------");
}

}

