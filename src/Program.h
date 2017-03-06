#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "InputHandler.h"
#include "RenderEngine.h"
#include "Renderable.h"
#include "ContentLoading.h"
#include "Camera.h"

class Program {

public:
	Program();
	virtual ~Program();

	void start();

private:
	GLFWwindow* window;
	RenderEngine* renderEngine;
	Camera* camera;

	static void error(int error, const char* description);
	void setupWindow();
	void loadObjects();
	void mainLoop();
};
