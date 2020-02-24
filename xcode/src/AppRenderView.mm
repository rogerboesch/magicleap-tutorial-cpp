
#import "AppRenderView.h"
#import "Game.hpp"
#import "RBCamera3D.hpp"
#import "RBLevelLoader.hpp"

Game game;
rb::Camera3D camera;

void dump(glm::mat4 m) {
	printf("%.03f,%.03f,%.03f,%.03f\n", m[0][0], m[0][1], m[0][2], m[0][3]);
	printf("%.03f,%.03f,%.03f,%.03f\n", m[1][0], m[1][1], m[1][2], m[1][3]);
	printf("%.03f,%.03f,%.03f,%.03f\n", m[2][0], m[2][1], m[2][2], m[2][3]);
	printf("%.03f,%.03f,%.03f,%.03f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

glm::mat4 frustum(float left, float right, float bottom, float top, float nearZ, float farZ) {
	float deltaX = right - left;
	float deltaY = top - bottom;
	float deltaZ = farZ - nearZ;
	glm::mat4 frust;
	
	if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
		(deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f)) {
		return glm::mat4(1);
	}
	
	frust[0][0] = 2.0f * nearZ / deltaX;
	frust[0][1] = frust[0][2] = frust[0][3] = 0.0f;
	
	frust[1][1] = 2.0f * nearZ / deltaY;
	frust[1][0] = frust[1][2] = frust[1][3] = 0.0f;
	
	frust[2][0] = (right + left) / deltaX;
	frust[2][1] = (top + bottom) / deltaY;
	frust[2][2] = -(nearZ + farZ) / deltaZ;
	frust[2][3] = -1.0f;
	
	frust[3][2] = -2.0f * nearZ * farZ / deltaZ;
	frust[3][0] = frust[3][1] = frust[3][3] = 0.0f;
	
	frust = frust * glm::mat4(1);
	return frust;
}

glm::mat4 perspective(float fovy, float aspect, float nearZ, float farZ) {
	float frustumW, frustumH;
	
	frustumH = tanf(fovy / 360.0f * M_PI) * nearZ;
	frustumW = frustumH * aspect;
	
	return frustum(-frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

@implementation AppRenderView

- (void)render {
	float width = self.bounds.size.width;
	float height = self.bounds.size.height;
	float aspect = width / height;

	// Set perspective and camera (view) matrix
	glm::mat4 projectionMatrix(1);
	projectionMatrix = perspective(45.0f, aspect, 0.1f, 200.0f);
	glm::mat4 viewMatrix = camera.GetViewMatrix();

	game.SetProjectionMatrix(projectionMatrix*viewMatrix);
	game.SetCameraPosition(camera.GetPosition());
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	game.Update();
	game.Render();
}

- (void)initialize:(CGSize)size {
	std::string path = [[[NSBundle mainBundle] resourcePath] UTF8String];
    rb::Shader::SetShaderPath(path);
    rb::LevelLoader::SetResourcePath(path);

	glViewport(0, 0, size.width, size.height);

	//glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);

	// Default camera (just needed for macOS)
	camera.SetPosition(glm::vec3(1.2, 0.3, 2.8));
	camera.SetLookAt(glm::vec3(0, 0, -5));

	game.Initialize();
}

- (void)resize:(CGSize)size {
	glViewport(0, 0, size.width, size.height);
}

- (void)processKey:(int)keyCode pressDown:(bool)down shiftPressed:(bool)shiftPressed {
	int vkCode = -1;
	float factor = 0.3;
	
	if (shiftPressed) {
		switch (keyCode) {
			case 18:   // 1
				camera.MoveBy(glm::vec3(0,factor,0));
				break;
				
			case 19:   // 2
				camera.MoveBy(glm::vec3(0,-factor,0));
				break;
				
			case 49:   // Space
				camera.MoveTo(glm::vec3(0,1,0));
				break;

			case 123:   // Left
				camera.MoveBy(glm::vec3(-factor,0,0));
				break;

			case 124:   // Right
				camera.MoveBy(glm::vec3(factor,0,0));
				break;
				
			case 125:   // Down
				camera.MoveBy(glm::vec3(0,0,factor));
				break;
				
			case 126:   // Up
				camera.MoveBy(glm::vec3(0,0,-factor));
				break;
				
			default:
				return;
		}

		return;
	}
	
    switch (keyCode) {
		case 18:   // 1
			RB_LOG(Debug, "Press 'button 1'");
			vkCode = Key::Button1;
			break;
			
		case 19:   // 2
			RB_LOG(Debug, "Press 'button 2'");
			vkCode = Key::Button2;
			break;
			
		case 20:   // 3
			RB_LOG(Debug, "Press 'button 3'");
			vkCode = Key::Button3;
			break;
		case 21:   // 4
			RB_LOG(Debug, "Press 'button 4'");
			vkCode = Key::Button4;
			break;

        case 49:   // Space
            RB_LOG(Debug, "Press 'select'");
            vkCode = Key::Select;
            break;

        case 51:   // Backspace
            RB_LOG(Debug, "Press 'back'");
            vkCode = Key::Back;
            break;

		case 123:   // Left
			RB_LOG(Debug, "Press 'left'");
			vkCode = Key::Left;
			break;

		case 124:   // Right
			RB_LOG(Debug, "Press 'right'");
			vkCode = Key::Right;
			break;
			
		case 125:   // Down
			RB_LOG(Debug, "Press 'down'");
			vkCode = Key::Down;
			break;
			
		case 126:   // Up
			RB_LOG(Debug, "Press 'up'");
			vkCode = Key::Up;
			break;
			
		default:
			return;
    }

	if (down) {
		game.PressVirtualKey(vkCode);
	}
	else {
		NSLog(@"Virtual key released: %d", vkCode);
		game.ReleaseVirtualKey(vkCode);
	}
}

@end
