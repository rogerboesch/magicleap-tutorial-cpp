
#include "RBPrimitive.hpp"

#define NUM_STEPS 30
#define NUM_VERTS (NUM_STEPS + 1) * 2
#define BOTTOM_RADIUS 0.5
#define TOP_RADIUS 0.5

static const GLfloat verticesCube[] = {
   -0.5f, 0.5f, 0.5f,     //  0: Front-top-left
   0.5f, 0.5f, 0.5f,      //  1: Front-top-right
   -0.5f, -0.5f, 0.5f,    //  2: Front-bottom-left
   0.5f, -0.5f, 0.5f,     //  3: Front-bottom-right
   0.5f, -0.5f, -0.5f,    //  4: Back-bottom-right
   0.5f, 0.5f, 0.5f,      //  5: Front-top-right
   0.5f, 0.5f, -0.5f,     //  6: Back-top-right
   -0.5f, 0.5f, 0.5f,     //  7: Front-top-left
   -0.5f, 0.5f, -0.5f,    //  8: Back-top-left
   -0.5f, -0.5f, 0.5f,    //  9: Front-bottom-left
   -0.5f, -0.5f, -0.5f,   // 10: Back-bottom-left
   0.5f, -0.5f, -0.5f,    // 11: Back-bottom-right
   -0.5f, 0.5f, -0.5f,    // 12: Back-top-left
   0.5f, 0.5f, -0.5f      // 13: Back-top-right
};

static const GLfloat verticesPyramide[] = {
	0.0f, 1.f, 0.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 1.0f,
	0.0f, -1.0f, -1.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	0.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f
};

static const GLfloat verticesCylinder[] = {
	0.500000, 0.000000, 0.000000,
	0.500000, 1.000000, 0.000000,
	0.433013, 0.000000, 0.250000,
	0.433013, 1.000000, 0.250000,
	0.250000, 0.000000, 0.433013,
	0.250000, 1.000000, 0.433013,
	-0.000000, 0.000000, 0.500000,
	-0.000000, 1.000000, 0.500000,
	-0.250000, 0.000000, 0.433013,
	-0.250000, 1.000000, 0.433013,
	-0.433013, 0.000000, 0.250000,
	-0.433013, 1.000000, 0.250000,
	-0.500000, 0.000000, 0.000000,
	-0.500000, 1.000000, 0.000000,
	-0.433013, 0.000000, -0.250000,
	-0.433013, 1.000000, -0.250000,
	-0.250000, 0.000000, -0.433013,
	-0.250000, 1.000000, -0.433013,
	-0.000000, 0.000000, -0.500000,
	-0.000000, 1.000000, -0.500000,
	0.250000, 0.000000, -0.433013,
	0.250000, 1.000000, -0.433013,
	0.433013, 0.000000, -0.250000,
	0.433013, 1.000000, -0.250000,
	0.500000, 0.000000, -0.000000,
	0.500000, 1.000000, -0.000000
};

static const GLfloat verticesCone[] = {
	0.500000, 0.000000, 0.000000,
	0.000000, 1.000000, 0.000000,
	0.433013, 0.000000, 0.250000,
	0.000000, 1.000000, 0.000000,
	0.250000, 0.000000, 0.433013,
	0.000000, 1.000000, 0.000000,
	-0.000000, 0.000000, 0.500000,
	-0.000000, 1.000000, 0.000000,
	-0.250000, 0.000000, 0.433013,
	-0.000000, 1.000000, 0.000000,
	-0.433013, 0.000000, 0.250000,
	-0.000000, 1.000000, 0.000000,
	-0.500000, 0.000000, 0.000000,
	-0.000000, 1.000000, 0.000000,
	-0.433013, 0.000000, -0.250000,
	-0.000000, 1.000000, -0.000000,
	-0.250000, 0.000000, -0.433013,
	-0.000000, 1.000000, -0.000000,
	-0.000000, 0.000000, -0.500000,
	-0.000000, 1.000000, -0.000000,
	0.250000, 0.000000, -0.433013,
	0.000000, 1.000000, -0.000000,
	0.433013, 0.000000, -0.250000,
	0.000000, 1.000000, -0.000000,
	0.500000, 0.000000, -0.000000,
	0.000000, 1.000000, -0.000000
};

inline float degreesToRadian(float deg) {
	return M_PI * deg / 180.0;
}

namespace rb {

GLuint Primitive::_cubeVaoID = -1;
GLuint Primitive::_cylinderVaoID = -1;
GLuint Primitive::_coneVaoID = -1;
GLuint Primitive::_pyramidVaoID = -1;

// -----------------------------------------------------------------------------
#pragma mark - Primitive support

Primitive::Primitive(Shape3D shape) : Actor() {
	_position = glm::vec3(0.0);
	_rotation = glm::vec3(0.0);
	_rotationSpeed = glm::vec3(0.0);
	_scale = glm::vec3(1.0);
	_speed = glm::vec3(0.0);
	
	_color = COLOR_RED;
	_shape = shape;

	RB_LOG(Debug, "Primitive %lu added", GetID());
}

GLuint Primitive::_CreateVAO(Shader shader, GLfloat* vertices, int size) {
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
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glUseProgram(0);

	RB_LOG(Debug, "Create vertex array buffer (VBO) for location '%d' in primitive '%lu'", vao, GetID());

	return vao;
}

void Primitive::_CreateAllVAOs(Shader shader) {
	_cubeVaoID = _CreateVAO(shader, (GLfloat*)&verticesCube[0], sizeof(verticesCube));
	_cylinderVaoID = _CreateVAO(shader, (GLfloat*)&verticesCylinder[0], sizeof(verticesCylinder));
	_coneVaoID = _CreateVAO(shader, (GLfloat*)&verticesCone[0], sizeof(verticesCone));
	_pyramidVaoID = _CreateVAO(shader, (GLfloat*)&verticesPyramide[0], sizeof(verticesPyramide));
}

void Primitive::Update(float delta) {
	_position.x += _speed.x*delta;
	_position.y += _speed.y*delta;
	_position.z += _speed.z*delta;

	_rotation.x += _rotationSpeed.x * delta;
	_rotation.y += _rotationSpeed.y * delta;
	_rotation.z += _rotationSpeed.z * delta;
}

void Primitive::RenderInternal(Shader& shader, glm::mat4 projectionMatrix) {
	if (IsHidden()) return;

	shader.Use();
	shader.ApplyColor(_color);
	shader.ApplyProjection(projectionMatrix);

	switch (_shape) {
	case Pyramide:
		shader.ApplyAndDrawVertices((GLfloat*)&verticesPyramide[0], 12);
		break;
	case Cylinder:
		shader.ApplyAndDrawVertices((GLfloat*)&verticesCylinder[0], 26);
		break;
	case Cone:
		shader.ApplyAndDrawVertices((GLfloat*)&verticesCone[0], 26);
		break;
	default:
		shader.ApplyAndDrawVertices((GLfloat*)&verticesCube[0], 14);
		break;
	}
}

void Primitive::Render(Shader& shader, glm::mat4 projectionMatrix) {
	if (IsHidden()) return;

	glm::mat4 mT = glm::translate(glm::mat4(1.0f), _position);
	glm::mat4 mS = glm::scale(_scale);
	
	glm::mat4 mR = glm::rotate(glm::mat4(1.0f), degreesToRadian(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	mR = glm::rotate(mR, degreesToRadian(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mR = glm::rotate(mR, degreesToRadian(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 m = mT * mR * mS;
	_transform = m;
	
	RenderTest(shader, projectionMatrix);
}

void Primitive::RenderDirect(Shader& shader, glm::mat4 projectionMatrix) {
	if (IsHidden()) return;

	glm::mat4 m = projectionMatrix * _transform;

	RenderInternal(shader, m);
}

void Primitive::RenderTest(Shader& shader, glm::mat4 projectionMatrix) {
	glm::mat4 m = projectionMatrix * _transform;

	if (_cubeVaoID == -1) {
		_CreateAllVAOs(shader);		
	}

	shader.Use();

	shader.ApplyColor(_color);
	shader.ApplyProjection(m);

	switch (_shape) {
	case Pyramide:
		shader.DrawVBO(_pyramidVaoID, 12);
		break;
	case Cylinder:
		shader.DrawVBO(_cylinderVaoID, 26);
		break;
	case Cone:
		shader.DrawVBO(_coneVaoID, 26);
		break;
	default:
		shader.DrawVBO(_cubeVaoID, 14);
		break;
	}

}

// -----------------------------------------------------------------------------
#pragma mark - Transform support

void Primitive::ScaleTo(float scale) {
	_scale.x = scale;
	_scale.y = scale;
	_scale.z = scale;
}

glm::vec3 Primitive::GetSize() {
	return glm::vec3(1.0*_scale.x/2.0, 1.0*_scale.y/2.0, 1.0*_scale.z/2.0);
}

bool Primitive::TestCollisionAABB(Actor* actor) {
	Primitive* primitive = dynamic_cast<Primitive*>(actor);
	if (!primitive) {
		return false;
	}
	
	glm::vec3 myPosition = GetPosition();
	glm::vec3 otherPosition = primitive->GetPosition();
	glm::vec3 mySize = GetSize();
	glm::vec3 otherSize = primitive->GetSize();

	// Check X-axis
	if (abs(myPosition.x - otherPosition.x) < mySize.x + otherSize.x) {
		// Check Y-axis
		if (abs(myPosition.y - otherPosition.y) < mySize.y + otherSize.y) {
			// Check Z-axis
			if (abs(myPosition.z - otherPosition.z) < mySize.z + otherSize.z) {
				RB_LOG(Debug, "Position: %f,%f,%f <-> %f,%f,%f", myPosition.x, myPosition.y, myPosition.z, otherPosition.x, otherPosition.y, otherPosition.z);
				RB_LOG(Debug, "Size: %f,%f,%f <-> %f,%f,%f", mySize.x, mySize.y, mySize.z, otherSize.x, otherSize.y, otherSize.z);

				return true;
			}
		}
	}

	return false;
}

void Primitive::Dump() {
	RB_LOG(Debug, "------------ Dump of primitive #%03d -------------", (int)GetID());
	RB_LOG(Debug, "GLUid ID: %d,%d,%d,%d", _cubeVaoID, _cylinderVaoID, _coneVaoID, _pyramidVaoID);
	RB_LOG(Debug, "     Tag: %d", GetTag());
	RB_LOG(Debug, "  Hidden: %d", IsHidden());
	RB_LOG(Debug, "   Color: %f,%f,%f", _color[0], _color[1], _color[2]);
	RB_LOG(Debug, "   Shape: %d", _shape);
	RB_LOG(Debug, "Position: %f,%f,%f", _position.x, _position.y, _position.z);
	RB_LOG(Debug, "Rotation: %f,%f,%f", _rotation.x, _rotation.y, _rotation.z);
	RB_LOG(Debug, "   Scale: %f,%f,%f", _scale.x, _scale.y, _scale.z);
	RB_LOG(Debug, "   Speed: %f,%f,%f", _speed.x, _speed.y, _speed.z);
	RB_LOG(Debug, "RotSpeed: %f,%f,%f", _rotationSpeed.x, _rotationSpeed.y, _rotationSpeed.z);
	RB_LOG(Debug, "--------------------------------------------------");
}

}
