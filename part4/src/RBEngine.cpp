#include "RBEngine.hpp"

namespace rb {

// -----------------------------------------------------------------------------
#pragma mark - Engine main methods

void Engine::Initialize() {
	_shader3D = Shader();

#ifdef MACOS_OPENGL
	_shader3D.Load("macos3D.vert", "macos.frag");
#else
	_shader3D.Load("magicleap3D.vert", "magicleap.frag");
#endif
	
	_clock.Reset();
	OnCreateContent();
}

void Engine::Update() {
	float delta = _clock.Delta();
	
	for (auto actor : _actors) {
		actor->Update(delta);
		OnDetectCollision(actor);
	}
	
	OnUpdateContent(delta);
}

void Engine::Render() {
    // Part 4: Sort back-to-front
	std::sort(_actors.begin(), _actors.end(), [this](Actor* a, Actor* b) {
		rb::Primitive* primitiveA = dynamic_cast<rb::Primitive*>(a);
		rb::Primitive* primitiveB = dynamic_cast<rb::Primitive*>(b);

		const auto dist1 = glm::distance(_cameraPosition, primitiveA->GetPosition());
		const auto dist2 = glm::distance(_cameraPosition, primitiveB->GetPosition());

		return dist1 > dist2;
	});

	for (auto actor : _actors) {
		actor->Render(GetShader3D(), _projectionMatrix);
	}

	OnRenderContent(_projectionMatrix);
}

void Engine::Stop() {
	_state = ENGINE_STOPPED;
	_clock.Reset();
}

void Engine::Pause() {
	_state = ENGINE_PAUSED;
	_clock.Reset();
}

void Engine::Resume() {
	_state = ENGINE_PLAY;
	_clock.Reset();
}

void Engine::ShowAllActors() {
	for (auto actor : _actors) {
		actor->SetHidden(false);
	}
}

void Engine::PressVirtualKey(int code) {
	if (_keys[code] == 0) {
		_keys[code] = 1;
	}
	else if (_keys[code] == 1) {
		_keys[code] = 2;
	}
}

void Engine::ReleaseVirtualKey(int code) {
	_keys[code] = 0;
}

bool Engine::IsVirtualKeyPressed(int code) {
	if (_keys[code] == 1)
		return true;
	
	return false;
}

bool Engine::IsVirtualKeyContinuePressed(int code) {
	if (_keys[code] != 0)
		return true;
	
	return false;
}

}

