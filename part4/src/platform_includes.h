#pragma once

#ifdef MACOS_OPENGL
	#import <OpenGL/gl.h>
	#import <OpenGL/OpenGL.h>

	#define glBindVertexArray glBindVertexArrayAPPLE
	#define glDeleteVertexArraysAPPLE
	#define glGenVertexArrays glGenVertexArraysAPPLE
	#define glIsVertexArrayAPPLE

	typedef enum _MLLogLevel {
		Unknown = 0,
		Default,
		Verbose,
		Debug,
		Info,
		Warn,
		Error,
		Fatal,
		Silent,
	} RBLogLevel;

	#if !defined(RB_DEFAULT_LOG_TAG)
		#define RB_DEFAULT_LOG_TAG "glide"
	#endif

	#define RB_LOG_FULL(level, cond, tag, ...)             \
	  do {                                                 \
		if ((cond) && RBLoggingLogLevelIsEnabled(level)) {           \
			RBLoggingLogVargs(level, tag, __VA_ARGS__);              \
		}                                                  \
	  } while (false)

	#define RB_LOG_TAG(level, tag, ...) RB_LOG_FULL(level, true, tag, __VA_ARGS__)
	#define RB_LOG(level, ...) RB_LOG_FULL(level, true, RB_DEFAULT_LOG_TAG, __VA_ARGS__)

	bool RBLoggingLogLevelIsEnabled(RBLogLevel level);
	void RBLoggingLogVargs(RBLogLevel level, const char *tag, const char *fmt, ...);

#else
	#include <EGL/egl.h>
	#include <EGL/eglext.h>
	#include <GLES3/gl3.h>
	#include <GLES3/gl3ext.h>

	#include <ml_logging.h>
	#include <ml_graphics.h>
	#include <ml_lifecycle.h>
	#include <ml_perception.h>
	#include <ml_head_tracking.h>
	#include <ml_input.h>
	#include <ml_privileges.h>

	#define RB_LOG ML_LOG
#endif

#define GLM_ENABLE_EXPERIMENTAL 1
#include "../glm/gtx/quaternion.hpp"

#include "../glm/glm.hpp"
#include "../glm/ext.hpp"
#include "../glm/gtx/vector_angle.hpp"
#include "../glm/gtx/transform.hpp" 
#include "../glm/gtc/matrix_transform.hpp"


// -----------------------------------------------------------------------------
// Part 3: Add some colors

#define COLOR_RED 		glm::vec3(1,0,0)
#define COLOR_GREEN 	glm::vec3(0,1,0)
#define COLOR_BLUE 		glm::vec3(0,0,1)
#define COLOR_BLACK 	glm::vec3(0,0,0)
#define COLOR_GRAY 		glm::vec3(0.5,0.5,0.5)
#define COLOR_WHITE 	glm::vec3(1,1,1)
#define COLOR_YELLOW 	glm::vec3(1,1,0)
#define COLOR_CYAN 		glm::vec3(0,1,1)
#define COLOR_PINK      glm::vec3(1,0,1)
#define COLOR_ORANGE    glm::vec3(0.96,0.57,0.1)

#define APP_COLOR_LASERPOINTER COLOR_PINK

extern void dump(glm::mat4 m);

namespace Key {

typedef enum {
    // Menu/General
    Select = 0,
    Back,
    
    // Joystick/Keys
	Left,
	Right,
	Up,
	Down,
	Button1,
	Button2,
	Button3,
	Button4,
	TotalKeys
} VirtualKey;

}

namespace Direction {

typedef enum {
	None = 0,
	Left,
	Right,
	Up,
	Down,
} VirtualDirection;

}
