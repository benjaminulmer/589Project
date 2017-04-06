#include "InputHandler.h"

Camera* InputHandler::camera;
RenderEngine* InputHandler::renderEngine;
Program* InputHandler::program;
float InputHandler::mouseOldX;
float InputHandler::mouseOldY;

// Must be called before processing any GLFW events
void InputHandler::setUp(Camera* camera, RenderEngine* renderEngine, Program* program) {
	InputHandler::camera = camera;
	InputHandler::renderEngine = renderEngine;
	InputHandler::program = program;
}

void InputHandler::pollEvent(SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		InputHandler::key(e.key);
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN) {
		InputHandler::mouse(e.button);
	}
	else if (e.type == SDL_MOUSEMOTION) {
		InputHandler::motion(e.motion);
	}
	else if (e.type == SDL_MOUSEWHEEL) {
		InputHandler::scroll(e.wheel);
	}
	else if (e.type == SDL_WINDOWEVENT) {
		InputHandler::reshape(e.window);
	}
	else if (e.type == SDL_QUIT) {
		SDL_Quit();
		exit(0);
	}
}

// Callback for key presses
void InputHandler::key(SDL_KeyboardEvent& e) {
	// Light controls
	switch (e.keysym.sym) {
	case(SDLK_w) :
		renderEngine->updateLightPos(glm::vec3(0.0, 0.1, 0.0));
		break;
	case(SDLK_s) :
		renderEngine->updateLightPos(glm::vec3(0.0, -0.1, 0.0));
		break;
	case(SDLK_a) :
		renderEngine->updateLightPos(glm::vec3(-0.1, 0.0, 0.0));
		break;
	case(SDLK_d) :
		renderEngine->updateLightPos(glm::vec3(0.1, 0.0, 0.0));
		break;
	case(SDLK_e) :
		renderEngine->updateLightPos(glm::vec3(0.0, 0.0, 0.1));
		break;
	case(SDLK_q) :
		renderEngine->updateLightPos(glm::vec3(0.0, 0.0, -0.1));
		break;
	case(SDLK_1) :
		program->setState(State::EXPLODE);
		break;
	case(SDLK_2) :
		program->setState(State::COLLAPSE);
		break;
	case(SDLK_r) :
		program->updateDistanceBuffer(0.1f);
		break;
	case(SDLK_f) :
		program->updateDistanceBuffer(-0.1f);
		break;
	case(SDLK_t) :
		program->updateExplosionTime(0.25f);
		break;
	case(SDLK_g) :
		program->updateExplosionTime(-0.25f);
		break;
	case(SDLK_z) :
		program->moveCurrentPart(-0.25f);
		break;
	case(SDLK_x) :
		program->moveCurrentPart(0.25f);
		break;
	case(SDLK_ESCAPE) :
		SDL_Quit();
		exit(0);
	}
}

// Callback for mouse button presses
void InputHandler::mouse(SDL_MouseButtonEvent& e) {
	mouseOldX = e.x;
	mouseOldY = e.y;
}

// Callback for mouse motion
void InputHandler::motion(SDL_MouseMotionEvent& e) {
	double dx, dy;
	dx = (e.x - mouseOldX);
	dy = (e.y - mouseOldY);

	// left mouse button moves camera
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		camera->updateLongitudeRotation(dx * 0.5);
		camera->updateLatitudeRotation(dy * 0.5);
	}

	// Update current position of the mouse
	int width, height;
	SDL_Window* window = SDL_GetWindowFromID(e.windowID);
	SDL_GetWindowSize(window, &width, &height);

	int iX = (int)e.x;
	int iY = height - (int)e.y;
	program->setMousePos(iX, iY);

	mouseOldX = e.x;
	mouseOldY = e.y;
}

// Callback for mouse scroll
void InputHandler::scroll(SDL_MouseWheelEvent& e) {
	double dy;
	dy = (e.x - e.y);
	camera->updatePosition(glm::vec3(dy * -1.0f, 0.0, 0.0));
}

// Callback for window reshape/resize
void InputHandler::reshape(SDL_WindowEvent& e) {
	if (e.event == SDL_WINDOWEVENT_RESIZED) {
		program->setWindowSize(e.data1, e.data2);
	}
}
