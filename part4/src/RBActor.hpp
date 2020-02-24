
#pragma once

#include "platform_includes.h"
#include "RBShader.hpp"

namespace rb {
   
class Actor {
public:
	Actor();

// Game loop related
public:	
	virtual void Update(float delta);
	virtual void Render(Shader& shader, glm::mat4 projectionMatrix);
	virtual bool TestCollisionAABB(Actor* actor) { return false; }

// Actor related
public:
	void SetHidden(bool flag) { _hidden = flag; }
	bool IsHidden() { return _hidden; }
	void SetTag(int tag) { _tag = tag; }
	int GetTag() { return _tag; }
	long GetID() { return _id; }
	void SetID(long id) { _id = id; }

// Actor related
private:
	static long _counter;
	long _id;
	int _tag;
	bool _hidden;
};
    
}

