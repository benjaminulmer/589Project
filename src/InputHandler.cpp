#include "InputHandler.h"

Camera* InputHandler::camera;
RenderEngine* InputHandler::renderEngine;
float InputHandler::mouseOldX;
float InputHandler::mouseOldY;

// Must be called before processing any GLFW events
void InputHandler::setUp(Camera* camera, RenderEngine* renderEngine) {
	InputHandler::camera = camera;
	InputHandler::renderEngine = renderEngine;
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
	else if (key == GLFW_KEY_ESCAPE) {
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);
	}
}

// Callback for mouse button presses
void InputHandler::mouse(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		mouseOldX = x;
		mouseOldY = y;
	}
}

// Callback for mouse motion
void InputHandler::motion(GLFWwindow* window, double x, double y) {
	double dx, dy;
	dx = (x - mouseOldX);
	dy = (y - mouseOldY);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
		camera->updateLongitudeRotation(dx * 0.5);
		camera->updateLatitudeRotation(dy * 0.5);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//translate_z += dy * 0.03f;
	}

	mouseOldX = x;
	mouseOldY = y;
}

// Callback for mouse scroll
void InputHandler::scroll(GLFWwindow* window, double x, double y) {
	double dy;
	dy = (x - y);
	camera->updatePosition(glm::vec3(0.0, 0.0, dy * 0.1));
}

// Callback for window reshape/resize
void InputHandler::reshape(GLFWwindow* window, int width, int height) {
	renderEngine->setWindowSize(width, height);
}
