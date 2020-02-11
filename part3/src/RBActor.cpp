
#include "RBActor.hpp"


namespace rb {

// -----------------------------------------------------------------------------
#pragma mark - Actor support

long Actor::_counter = 0;

Actor::Actor() {
	_hidden = false;
	_id = ++_counter;
	_tag = 0;
}

void Actor::Update(float delta) {}

void Actor::Render(Shader& shader, glm::mat4 projectionMatrix) {}

}

