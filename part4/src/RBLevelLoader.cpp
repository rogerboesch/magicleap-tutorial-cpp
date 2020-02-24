#include "RBLevelLoader.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "platform_includes.h"

// -----------------------------------------------------------------------------
#pragma mark - Level

namespace rb {

std::string LevelLoader::s_resourcePath = "res";

LevelLoader::LevelLoader() {
	_width = -1;
	_height = -1;
	_content = "";
}

char LevelLoader::GetTokenAt(int x, int y) {
	if (_width < 1 || _height < 1) {
		// Empty
		return '.';
	}
	if (x >= _width || y >= _height) {
		// Empty
		return '.';
	}

	int offset = y*_width+x;
	return _content.at(offset);
}

bool LevelLoader::Load(std::string filename) {
	std::string fullPath = s_resourcePath + "/" + filename;
	
	std::ifstream contentStream(fullPath, std::ios::in);

	_width = -1;
	_height = 0;
	_content = "";
	
	if (contentStream.is_open()) {
		std::string line = "";

		while (getline(contentStream, line)) {
			if (_width == -1) {
				_width = (int)line.size();
			}
			else if (_width != (int)line.size()) {
				RB_LOG(Error, "All rows must have same width: %d", _width);
				return false;
			}
			
			_content += line;
			_height++;
		}

		contentStream.close();
	}
	else {
		RB_LOG(Error, "Can't open level file '%s'", fullPath.c_str());
		return false;
	}

	RB_LOG(Debug, "Level loaded. Size is %d x %d", _width, _height);
	return true;
}

}
