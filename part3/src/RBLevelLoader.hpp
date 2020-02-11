#pragma once

#include <string>

namespace rb {

class LevelLoader {
public:
	LevelLoader();
	~LevelLoader() {}

public:
	bool Load(std::string filename);
	char GetTokenAt(int x, int y);
	int GetWidth() { return _width; }
	int GetHeight() { return _height; }
	
public:
	static void SetResourcePath(std::string path) { s_resourcePath = path; }
	
private:
	static std::string s_resourcePath;
	std::string _content;
	int _width;
	int _height;
};

}
