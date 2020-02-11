#define MLSDK20

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cmath>

#include "platform_includes.h"
#include "main.h"

#include "Game.hpp"

// -----------------------------------------------------------------------------
// App Lifecycle callback functions

static void on_stop(void* user_data) {
	RB_LOG(Info, "Lifecycle call on_stop()");
}

static void on_pause(void* user_data) {
	RB_LOG(Info, "Lifecycle call on_pause()");
}

static void on_resume(void* user_data) {
	RB_LOG(Info, "Lifecycle call on_resume()");
}

// -----------------------------------------------------------------------------
// Main

int main() {
	graphics_context_t graphics_context;

	MLLoggingEnableLogLevel(MLLogLevel_Debug);

	// Assign call application lifecycle callback functions
	MLLifecycleCallbacks lifecycle_callbacks = {};
	lifecycle_callbacks.on_stop = on_stop;
	lifecycle_callbacks.on_pause = on_pause;
	lifecycle_callbacks.on_resume = on_resume;

	// Initialize application lifecycle
	MLResult result = MLLifecycleInit(&lifecycle_callbacks, nullptr);

	if (result != MLResult_Ok) {
		RB_LOG(Error, "Failed to initialize lifecycle system");
		return -1;
	}
	else {
		RB_LOG(Debug, "Lifecycle system started");
	}

	// Initialize perception system
	MLPerceptionSettings perception_settings;
	if (MLResult_Ok != MLPerceptionInitSettings(&perception_settings)) {
		RB_LOG(Error, "Failed to initialize perception");
		return -1;
	}

	if (MLResult_Ok != MLPerceptionStartup(&perception_settings)) {
		RB_LOG(Error, "Failed to startup perception");
		return -1;
	}

	// Create OpenGL context and create framebuffer
	graphics_context.makeCurrent();
	glGenFramebuffers(1, &graphics_context.framebuffer_id);

	MLGraphicsOptions graphics_options = { 0, MLSurfaceFormat_RGBA8UNorm, MLSurfaceFormat_D32Float };
	MLHandle opengl_context = reinterpret_cast<MLHandle>(graphics_context.egl_context);
	MLHandle graphics_client = ML_INVALID_HANDLE;
	MLGraphicsCreateClientGL(&graphics_options, opengl_context, &graphics_client);

	// Set up the head tracker
	MLHandle head_tracker;
	MLResult head_track_result = MLHeadTrackingCreate(&head_tracker);
	MLHeadTrackingStaticData head_static_data;

	if (MLResult_Ok == head_track_result && MLHandleIsValid(head_tracker)) {
		MLHeadTrackingGetStaticData(head_tracker, &head_static_data);
	}
	else {
		RB_LOG(Error, "Failed to create head tracker");
	}

	// Ready for application lifecycle
	if (MLLifecycleSetReadyIndication() != MLResult_Ok) {
		RB_LOG(Error, "Failed to indicate lifecycle readyness");
		return -1;
	}
	else {
		RB_LOG(Debug, "Lifecycle system ready");
	}

	// Create the input handler
	RB_LOG(Debug, "Create input system (controller)");
	MLHandle input;
	CHECK(MLInputCreate(NULL, &input));
	
	// Part 3: Init game engine
	Game game;
	game.Initialize();
	
	// The game loop
	RB_LOG(Debug, "Enter game loop");

	// Part 3: If engine stops, end main loop, also support of Pause
	while (game.GetState() != rb::ENGINE_STOPPED) {
		if (game.GetState() != rb::ENGINE_PLAY) {
			continue;
		}
		
		// Get state of the Controller
		MLInputControllerState input_states[MLInput_MaxControllers];
        CHECK(MLInputGetControllerState(input, input_states));

        for (int k = 0; k < MLInput_MaxControllers; ++k) {
            if (input_states[k].is_connected) {
                if (input_states[k].button_state[MLInputControllerButton_Bumper]) {
					RB_LOG(Debug, "Press 'right'");
					game.PressVirtualKey(Key::Right);
                }
                else {
					game.ReleaseVirtualKey(Key::Right);
                }
            }
        }
			
		// Initialize a frame
		MLGraphicsFrameParams frame_params;
		result = MLGraphicsInitFrameParams(&frame_params);

		if (MLResult_Ok != result) {
			RB_LOG(Error, "MLGraphicsInitFrameParams() error: %d", result);
		}

		frame_params.surface_scale = 1.0f;
		frame_params.projection_type = MLGraphicsProjectionType_ReversedInfiniteZ;
		frame_params.near_clip = 0.38;
		frame_params.focus_distance = 1.0f;

		MLHandle frame_handle;
		MLGraphicsVirtualCameraInfoArray virtual_camera_array;

		// Begin the frame
		MLResult frame_result = MLGraphicsBeginFrame(graphics_client, &frame_params, &frame_handle, &virtual_camera_array);

		if (frame_result == MLResult_Ok) {
			// Prepare rendering for each camera/eye
			for (int camera = 0; camera < 2; ++camera) {
				glBindFramebuffer(GL_FRAMEBUFFER, graphics_context.framebuffer_id);
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, virtual_camera_array.color_id, 0, camera);
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, virtual_camera_array.depth_id, 0, camera);

				const MLRectf& viewport = virtual_camera_array.viewport;
				glViewport((GLint)viewport.x, (GLint)viewport.y, (GLsizei)viewport.w, (GLsizei)viewport.h);

				glClearColor(0.0, 0.0, 0.0, 0.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Get the projection matrix
				MLGraphicsVirtualCameraInfo &current_camera = virtual_camera_array.virtual_cameras[camera];
				glm::mat4 projectionMatrix = rb_projection_matrix(current_camera) * rb_camera_matrix(current_camera);

				// Part 3: Update and Render
				game.SetProjectionMatrix(projectionMatrix);
				game.Update();
				game.Render();
				
				// Bind the frame buffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				MLGraphicsSignalSyncObjectGL(graphics_client, virtual_camera_array.virtual_cameras[camera].sync_object);
			}

			// Finish the frame
			result = MLGraphicsEndFrame(graphics_client, frame_handle);

			if (MLResult_Ok != result) {
				RB_LOG(Error, "MLGraphicsEndFrame() error: %d", result);
			}
		}
		else if (frame_result != MLResult_Timeout) {
			// Sometimes it fails with timeout when device is busy
			RB_LOG(Error, "MLGraphicsBeginFrame() error: %d", frame_result);
		}
	}

	// End of game loop, clean app and exit
	RB_LOG(Debug, "End game loop");

	graphics_context.unmakeCurrent();

	glDeleteFramebuffers(1, &graphics_context.framebuffer_id);

	MLGraphicsDestroyClient(&graphics_client);

	RB_LOG(Debug, "System cleanup done");

	return 0;
}
