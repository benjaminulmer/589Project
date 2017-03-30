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

// Callback for key presses
void InputHandler::key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Light controls
	if (key == GLFW_KEY_W) {
		renderEngine->updateLightPos(glm::vec3(0.0, 0.1, 0.0));
	}
	else if (key == GLFW_KEY_S) {
		renderEngine->updateLightPos(glm::vec3(0.0, -0.1, 0.0));
	}
	else if (key == GLFW_KEY_A) {
		renderEngine->updateLightPos(glm::vec3(-0.1, 0.0, 0.0));
	}
	else if (key == GLFW_KEY_D) {
		renderEngine->updateLightPos(glm::vec3(0.1, 0.0, 0.0));
	}
	else if (key == GLFW_KEY_E) {
		renderEngine->updateLightPos(glm::vec3(0.0, 0.0, 0.1));
	}
	else if (key == GLFW_KEY_Q) {
		renderEngine->updateLightPos(glm::vec3(0.0, 0.0, -0.1));
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		program->setState(State::EXPLODE);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		program->setState(State::COLLAPSE);
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		program->updateDistanceBuffer(0.1f);
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		program->updateDistanceBuffer(-0.1f);
	}
	else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		program->updateExplosionTime(0.1f);
	}
	else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		program->updateExplosionTime(-0.1f);
	}

	else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		program->moveCurrentPart(-0.25f);
	}
	else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		program->moveCurrentPart(0.25f);
	}

	else if (key == GLFW_KEY_ESCAPE) {
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);
	}
}

// Callback for mouse button presses
void InputHandler::mouse(GLFWwindow* window, int button, int action, int mods) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	mouseOldX = x;
	mouseOldY = y;
}

// Callback for mouse motion
void InputHandler::motion(GLFWwindow* window, double x, double y) {
	double dx, dy;
	dx = (x - mouseOldX);
	dy = (y - mouseOldY);

	// Right mouse moves camera
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
		camera->updateLongitudeRotation(dx * 0.5);
		camera->updateLatitudeRotation(dy * 0.5);
	}

	// Update current position of the mouse
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	int iX = (int)x;
	int iY = height - (int)y;
	program->setMousePos(iX, iY);

	mouseOldX = x;
	mouseOldY = y;
}

// Callback for mouse scroll
void InputHandler::scroll(GLFWwindow* window, double x, double y) {
	double dy;
	dy = (x - y);
	camera->updatePosition(glm::vec3(dy * -1.0f, 0.0, 0.0));
}

// Callback for window reshape/resize
void InputHandler::reshape(GLFWwindow* window, int width, int height) {
	program->setWindowSize(width, height);
}
