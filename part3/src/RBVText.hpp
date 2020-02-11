
#pragma once

#include "platform_includes.h"
#include "RBLines.hpp"
#include <string>

namespace rb {
   
class VText : public Actor {

public:
	VText();
	
	virtual void Update(float delta);
	virtual void Render(Shader& shader, glm::mat4 projectionMatrix);

	void Dump() { _lines.Dump(); }

// Text related
public:
	void SetText(std::string text);
	void Clear();

private:
	void CreateText();
	void MoveTo(float x, float y);
	void MoveBy(float dx, float dy);
	void LineBy(float dx, float dy);
	void AddChar(char ch);

// Transform related
public:
	void SetPosition(float x, float y, float z) { _lines.SetPosition(x, y, z); }
	void SetPosition(glm::vec3 position) { _lines.SetPosition(position); }
	glm::vec3 GetPosition() { return _lines.GetPosition(); }
	void SetScale(float scale) { _scale = scale; }

// Material related
public:
	void SetColor(float r, float g, float b, float a) { _lines.SetColor(r, g, b, 1.0); }
	void SetColor(glm::vec3 color) { _lines.SetColor(color); }
	glm::vec3 GetColor() { return _lines.GetColor(); }

private:
	rb::Lines _lines;
	std::string _text;
	float _cursorX;
	float _cursorY;
	float _scale;
};
    
}

