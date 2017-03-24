#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <map>

#include "InputHandler.h"
#include "RenderEngine.h"
#include "Renderable.h"
#include "ContentLoading.h"
#include "Camera.h"
#include "ExplosionGraph.h"

enum class State {
	NONE,
	EXPLODE,
	COLLAPSE
};

class Program {

public:
	Program();
	virtual ~Program();

	void start();
	void setState(State newState);
	void _3Dpick(int x, int y);

private:
	GLFWwindow* window;
	RenderEngine* renderEngine;
	Camera* camera;
	ExplosionGraph* graph;

	State state;

	unsigned int level;
	float counterS;
	float timeSPerLevel;
	float distBuffer;

	static void error(int error, const char* description);
	void setupWindow();
	void loadObjects();
	void mainLoop();

	void explode();
	void collapse();
};
