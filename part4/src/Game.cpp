#include "Game.hpp"
#include "RBLevelLoader.hpp"

#define MAX_OBSTACLES_HIT_ALLOWED 3

// -----------------------------------------------------------------------------
#pragma mark - Life Bar support

void Game::_CreateLifeBar() {
	_lifeBar[0]	= new rb::Primitive(rb::Primitive::Cube);
	_lifeBar[0]->SetPosition(0, 0, 0);
	_lifeBar[0]->SetTag(LIFEBAR);
	_lifeBar[0]->SetColor(COLOR_GREEN);
	_lifeBar[0]->ScaleTo(0.03, 0.01, 0.01);

	_lifeBar[1]	= new rb::Primitive(rb::Primitive::Cube);
	_lifeBar[1]->SetPosition(0, 0, 0);
	_lifeBar[1]->SetTag(LIFEBAR);
	_lifeBar[1]->SetColor(COLOR_ORANGE);
	_lifeBar[1]->ScaleTo(0.03, 0.01, 0.01);

	_lifeBar[2]	= new rb::Primitive(rb::Primitive::Cube);
	_lifeBar[2]->SetPosition(0, 0, 0);
	_lifeBar[2]->SetTag(LIFEBAR);
	_lifeBar[2]->SetColor(COLOR_RED);
	_lifeBar[2]->ScaleTo(0.03, 0.01, 0.01);
}

void Game::_UpdateLifeBar() {
	glm::vec3 pos = _player->GetPosition();
	_lifeBar[0]->SetPosition(pos.x-0.03, pos.y+0.1, pos.z);
	_lifeBar[1]->SetPosition(pos.x, pos.y+0.1, pos.z);
	_lifeBar[2]->SetPosition(pos.x+0.03, pos.y+0.1, pos.z);

	switch (_obstaclesHit) {
	case 0:
		_lifeBar[0]->SetColor(COLOR_GREEN);
		_lifeBar[1]->SetColor(COLOR_GREEN);
		_lifeBar[2]->SetColor(COLOR_GREEN);
		break;
	case 1:
		_lifeBar[0]->SetColor(COLOR_ORANGE);
		_lifeBar[1]->SetColor(COLOR_ORANGE);
		_lifeBar[2]->SetColor(COLOR_BLACK);
		break;
	case 2:
		_lifeBar[0]->SetColor(COLOR_RED);
		_lifeBar[1]->SetColor(COLOR_BLACK);
		_lifeBar[2]->SetColor(COLOR_BLACK);
		break;
	}
}

void Game::_RenderLifeBar(glm::mat4 projectionMatrix) {
    _lifeBar[0]->Render(GetShader3D(), projectionMatrix);
    _lifeBar[1]->Render(GetShader3D(), projectionMatrix);
    _lifeBar[2]->Render(GetShader3D(), projectionMatrix);
}

// -----------------------------------------------------------------------------
#pragma mark - Game logic

void Game::_SetDirection(Direction::VirtualDirection direction) {
	_direction = direction;

	switch (_direction) {
		case Direction::None:
			_player->SetSpeed(0, 0, 0);
			break;
		case Direction::Left:
			_player->SetSpeed(-_speed, 0, 0);
			break;
		case Direction::Right:
			_player->SetSpeed(_speed, 0, 0);
			break;
		case Direction::Up:
			_player->SetSpeed(0, 0, -_speed);
			break;
		case Direction::Down:
			_player->SetSpeed(0, 0, _speed);
			break;
	}
}

void Game::_TurnLeft() {
	switch (_direction) {
		case Direction::None:
			break;
		case Direction::Left:
			_SetDirection(Direction::Down);
			break;
		case Direction::Right:
			_SetDirection(Direction::Up);
			break;
		case Direction::Up:
			_SetDirection(Direction::Left);
			break;
		case Direction::Down:
			_SetDirection(Direction::Right);
			break;
	}
}

void Game::_TurnRight() {
	switch (_direction) {
		case Direction::None:
			break;
		case Direction::Left:
			_SetDirection(Direction::Up);
			break;
		case Direction::Right:
			_SetDirection(Direction::Down);
			break;
		case Direction::Up:
			_SetDirection(Direction::Right);
			break;
		case Direction::Down:
			_SetDirection(Direction::Left);
			break;
	}
}

void Game::_ChangeState(GameState newState) {
	switch (newState) {
		case GAME_STARTED:
			ShowAllActors();
			_player->SetPosition(_startPosition);
			_time = 0;
			_collected = 0;
			_obstaclesHit = 0;
			_play.SetHidden(false);
			_win.SetHidden(true);
			_loose.SetHidden(true);
            _score.SetHidden(true);
			break;
		case GAME_PLAY:
			_SetDirection(Direction::Up);
			_play.SetHidden(true);
			_loose.SetHidden(true);
			_win.SetHidden(true);
            _score.SetHidden(false);
			break;
		case GAME_WIN:
			_SetDirection(Direction::None);
			_win.SetHidden(false);
			_play.SetHidden(true);
			_loose.SetHidden(true);
            _score.SetHidden(false);
			_speed *= 1.3f;
			break;
		case GAME_LOOSE:
			_SetDirection(Direction::None);
			_loose.SetHidden(false);
			_play.SetHidden(true);
			_win.SetHidden(true);
            _score.SetHidden(false);
			break;
	}
	
	_state = newState;
	RB_LOG(Debug, "State changed to: %d", _state);
}

// -----------------------------------------------------------------------------
#pragma mark - Game content

void Game::OnDetectCollision(rb::Actor* actor) {
	if (_state != GAME_PLAY) {
		return;
	}
	
	if (actor->GetTag() == PLAYER) {
		return;
	}

	if (actor->IsHidden()) {
		return;
	}

	if (actor->GetTag() == -1) {
		return;
	}

	if (actor->TestCollisionAABB(_player)) {
		if (actor->GetTag() == COLLECTABLE && actor->IsHidden() == false) {
			actor->SetHidden(true);
			_collected++;
			
			if (_collectables == _collected) {
				_ChangeState(GAME_WIN);
			}
		}
		else if (actor->GetTag() == OBSTACLE && actor->IsHidden() == false) {
			rb::Primitive* primitive = dynamic_cast<rb::Primitive*>(actor);
			if (primitive) {
				primitive->SetColor(COLOR_RED);
				primitive->SetTag(-1);
				_obstaclesHit++;
                
				RB_LOG(Debug, "%d Obstacles hit", _obstaclesHit);
				
				if (_obstaclesHit == MAX_OBSTACLES_HIT_ALLOWED) {
					_ChangeState(GAME_LOOSE);
				}
			}
		}
	}
}

void Game::OnRenderContent(glm::mat4 projectionMatrix) {
	_win.Render(GetShader3D(), projectionMatrix);
	_play.Render(GetShader3D(), projectionMatrix);
    _loose.Render(GetShader3D(), projectionMatrix);
    _score.Render(GetShader3D(), projectionMatrix);

    _RenderLifeBar(projectionMatrix);
}

bool Game::_IsKeyPressed(int code) {
	if (!IsVirtualKeyPressed(code)) {
		return false;
	}
	
	PressVirtualKey(code); // Workaround to push to continue before next frame

	return true;
}

void Game::OnUpdateContent(float delta) {
	_time += 1.0 * delta;

    _UpdateLifeBar();

	if (_state == GAME_STARTED) {
		if (_IsKeyPressed(Key::Left)) {
			_ChangeState(GAME_PLAY);
		}
		else if (_IsKeyPressed(Key::Right)) {
			_ChangeState(GAME_PLAY);
		}
		
		return;
	}

	if (_state == GAME_PLAY) {
		if (_IsKeyPressed(Key::Left)) {
			_TurnLeft();
		}
		else if (_IsKeyPressed(Key::Right)) {
			_TurnRight();
		}

		return;
	}
	
	if (_state == GAME_WIN || _state == GAME_LOOSE) {
		if (_IsKeyPressed(Key::Left)) {
			_ChangeState(GAME_STARTED);
		}
		else if (_IsKeyPressed(Key::Right)) {
			_ChangeState(GAME_STARTED);
		}
		
		return;
	}
}

void Game::_CreateBlock(float x, float y, float z, glm::vec3 color, int tag) {
	rb::Primitive* primitive = new rb::Primitive(rb::Primitive::Cube);
	primitive->SetPosition(x, y, z);
	primitive->ScaleTo(_factor);
	primitive->SetColor(color);
	primitive->SetTag(tag);
	AddActor(primitive);
}

void Game::OnCreateContent() {
	RB_LOG(Debug, "OnCreateContent()");
	
	rb::LevelLoader level;
	if (!level.Load("level.txt")) {
		return;
	}

	_collectables = 0;
	
	int width = level.GetWidth();
	int height = level.GetHeight();
	float ground = -0.75;
	
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			char ch = level.GetTokenAt(i, j);
			
			float zPosition = -(height-j)*_factor;
			
			switch (ch) {
				case '0':
					_CreateBlock(i*_factor, ground, zPosition, COLOR_PINK, OBSTACLE);
					break;
				case '1':
					_CreateBlock(i*_factor, ground, zPosition, COLOR_PINK, OBSTACLE);
					_CreateBlock(i*_factor, ground+1*_factor, zPosition, COLOR_WHITE, OBSTACLE);
					_CreateBlock(i*_factor, ground+2*_factor, zPosition, COLOR_WHITE, OBSTACLE);
					_CreateBlock(i*_factor, ground+3*_factor, zPosition, COLOR_WHITE, OBSTACLE);
					break;
				case 'X':
					_CreateBlock(i*_factor, ground, zPosition, COLOR_GREEN, COLLECTABLE);
					_collectables++;
					break;
				case '#':
					_CreateBlock(i*_factor, ground, zPosition, COLOR_PINK, OBSTACLE);
					// Score position
					_textPosition = glm::vec3((i-10)*_factor, ground+1*_factor, zPosition);
					break;
				case '@':
					// Player
					_player = new rb::Primitive(rb::Primitive::Cube);
					_player->SetPosition(i*_factor, ground, zPosition);
					_player->SetTag(PLAYER);
					_player->SetColor(COLOR_CYAN);
					_player->ScaleTo(_factor);
					break;
			}
		}
	}

	_play.SetText("START GAME");
	_play.SetHidden(true);
	_play.SetPosition(_textPosition);

	_loose.SetText("YOU LOST!");
	_loose.SetHidden(true);
	_loose.SetPosition(_textPosition);

    _win.SetText("YOU WIN!");
    _win.SetHidden(true);
    _win.SetPosition(_textPosition);

	if (_player != nullptr) {
		AddActor(_player);

		glm::vec3 pos = _player->GetPosition();
		_startPosition = pos;

		_CreateLifeBar();
	}
	else {
		RB_LOG(Error, "No player object ('@') found in level file");
		return;
	}
	
	RB_LOG(Info, "Level contains %d collectables", _collectables);

	_ChangeState(GAME_STARTED);
}
