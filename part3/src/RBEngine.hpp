#pragma once

#include <map>
#include <vector>
#include <string>

#include "platform_includes.h"
#include "RBShader.hpp"
#include "RBPrimitive.hpp"
#include "RBClock.hpp"

namespace rb {

typedef enum _EngineState {
	ENGINE_PLAY, ENGINE_PAUSED, ENGINE_STOPPED
} EngineState;

class Engine {
public:
	Engine() {}
	~Engine() {}

public:
	void Initialize();
	void Update();
	void Render();

	void Stop();
	void Pause();
	void Resume();

	virtual void OnCreateContent() {}
	virtual void OnUpdateContent(float delta) {}
	virtual void OnRenderContent(glm::mat4 projectionMatrix) {}
	virtual void OnDetectCollision(rb::Actor* actor) {}

	Shader& GetShader3D() { return _shader3D; }
	void SetProjectionMatrix(glm::mat4 matrix) { _projectionMatrix = matrix; }

	EngineState GetState() { return _state; }

	void AddActor(Actor* actor) { _actors.push_back(actor); }
	void InsertActor(Actor* actor) { _actors.insert(_actors.begin(), actor); }
	Actor* GetActorAtIndex(int index) { return _actors.at(index); }
	long NumberOfActors() { return _actors.size(); }
	void ShowAllActors();
	
	void PressVirtualKey(int code);
	void ReleaseVirtualKey(int code);
	bool IsVirtualKeyPressed(int code);
	bool IsVirtualKeyContinuePressed(int code);

private:
	Shader _shader3D;
	glm::mat4 _projectionMatrix;
	EngineState _state = ENGINE_PLAY;
	Clock _clock;
	std::vector<Actor*> _actors;
	int _keys[Key::TotalKeys];
};

}
