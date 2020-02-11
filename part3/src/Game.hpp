#pragma once

#include "RBEngine.hpp"
#include "RBPrimitive.hpp"
#include "RBVText.hpp"

typedef enum _GameState {
	GAME_STARTED, GAME_PLAY, GAME_WIN, GAME_LOOSE
} GameState;

typedef enum _ObjectType {
	COLLECTABLE, OBSTACLE, PLAYER
} ObjectType;

class Game : public rb::Engine {
public:
	Game() {}
	~Game() {}

public:
	virtual void OnCreateContent();
	virtual void OnUpdateContent(float delta);
	virtual void OnRenderContent(glm::mat4 projectionMatrix);
	virtual void OnDetectCollision(rb::Actor* actor);

private:
	void _CreateBlock(float x, float y, float z, glm::vec3 color, int tag);
	void _SetDirection(Direction::VirtualDirection direction);
	void _TurnLeft();
	void _TurnRight();
	void _ChangeState(GameState newState);
	bool _IsKeyPressed(int code);

private:
	float _speed = 0.1f*1.3f;
	float _factor = 0.05;
	float _time = 0.0;
	int _collectables;
	int _collected;
	int _obstaclesHit;
	rb::Primitive* _player;
	glm::vec3 _startPosition;
	glm::vec3 _textPosition;
	Direction::VirtualDirection _direction;
	GameState _state = GAME_STARTED;
	rb::VText _play;
	rb::VText _loose;
	rb::VText _win;
};
