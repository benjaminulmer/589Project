#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "RenderEngine.h"
#include "Program.h"

class Program;

class InputHandler {

public:
	static void setUp(Camera* camera, RenderEngine* renderEngine, Program* program);

	static void key(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse(GLFWwindow* window, int button, int action, int mods);
	static void motion(GLFWwindow* window, double x, double y);
	static void scroll(GLFWwindow* window, double x, double y);
	static void reshape(GLFWwindow* window, int width, int height);

private:
	static Camera* camera;
	static RenderEngine* renderEngine;
	static Program* program;

	static float mouseOldX;
	static float mouseOldY;
};
