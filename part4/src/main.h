#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cmath>

#ifndef EGL_EGLEXT_PROTOTYPES
	#define EGL_EGLEXT_PROTOTYPES
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES
#endif

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include <ml_graphics.h>
#include <ml_head_tracking.h>
#include <ml_perception.h>
#include <ml_input.h>
#include <ml_lifecycle.h>
#include <ml_planes.h>
#include <ml_logging.h>

#include "platform_includes.h"

// -----------------------------------------------------------------------------
#pragma mark - Types and definitions

#define CHECK(c) do { \
    auto result = (c); \
    if (result != MLResult_Ok) { \
        RB_LOG(Error, "%s failed (%d).", #c, (int)result); \
        return -1; \
    } \
} while (0)

struct graphics_context_t {
	EGLDisplay egl_display;
	EGLContext egl_context;
	GLuint framebuffer_id;

	graphics_context_t();
	~graphics_context_t();

	void makeCurrent();
	void swapBuffers();
	void unmakeCurrent();
};

// -----------------------------------------------------------------------------
#pragma mark - OpenGL boilerplate code

graphics_context_t::graphics_context_t() {
	egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	EGLint major = 4;
	EGLint minor = 0;
	eglInitialize(egl_display, &major, &minor);
	eglBindAPI(EGL_OPENGL_API);

	EGLint config_attribs[] = {
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_ALPHA_SIZE, 0,
		EGL_DEPTH_SIZE, 24,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};

	EGLConfig egl_config = nullptr;
	EGLint config_size = 0;
	eglChooseConfig(egl_display, config_attribs, &egl_config, 1, &config_size);

	EGLint context_attribs[] = {
		EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
		EGL_CONTEXT_MINOR_VERSION_KHR, 0,
		EGL_NONE
	};

	egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
}

void graphics_context_t::makeCurrent() {
	eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context);
}

void graphics_context_t::unmakeCurrent() {
	eglMakeCurrent(NULL, EGL_NO_SURFACE, EGL_NO_SURFACE, NULL);
}

void graphics_context_t::swapBuffers() {
	// buffer swapping is implicit on device (MLGraphicsEndFrame)
}

graphics_context_t::~graphics_context_t() {
	eglDestroyContext(egl_display, egl_context);
	eglTerminate(egl_display);
}

// -----------------------------------------------------------------------------
#pragma mark - glm helper methods

glm::mat4 rb_convert_ml_to_glm(const MLTransform &ml) {
    glm::quat q;
    q.w = ml.rotation.w;
    q.x = ml.rotation.x;
    q.y = ml.rotation.y;
    q.z = ml.rotation.z;

    return glm::translate(glm::mat4(), glm::vec3(ml.position.x, ml.position.y, ml.position.z)) * glm::toMat4(q);
}

glm::mat4 rb_projection_matrix(MLGraphicsVirtualCameraInfo &camera) {
	return (glm::make_mat4(camera.projection.matrix_colmajor));
}

glm::mat4 rb_camera_matrix(MLGraphicsVirtualCameraInfo &camera) {
	glm::mat4 proj = glm::make_mat4(camera.projection.matrix_colmajor);
	glm::vec3 trans = glm::make_vec3(camera.transform.position.values);
	glm::mat4 rotMat = glm::mat4_cast(glm::make_quat(camera.transform.rotation.values));
	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), trans);
	glm::mat4 worldFromCamera = transMat * rotMat;

	return (glm::inverse(worldFromCamera));
}

glm::vec3 rb_camera_position(MLGraphicsVirtualCameraInfo &camera) {
	glm::vec3 trans = glm::make_vec3(camera.transform.position.values);

	return trans;
}

glm::mat4 rb_controller_matrix(MLInputControllerState &input_state) {
	glm::vec3 trans = glm::make_vec3(input_state.position.values);
	glm::mat4 position = glm::translate(glm::mat4(1.0f), trans);
	glm::mat4 rotation = glm::mat4_cast(glm::make_quat(input_state.orientation.values));
	
	return (position * rotation);
}

glm::vec3 rb_controller_position(MLInputControllerState &input_state) {
	glm::vec3 position = glm::make_vec3(input_state.position.values);
	return position;
}

glm::quat rb_controller_rotation(MLInputControllerState &input_state) {
	glm::quat rotation = glm::make_quat(input_state.orientation.values);
	return rotation;
}
