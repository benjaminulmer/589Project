#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "InputHandler.h"
#include "RenderEngine.h"
#include "Camera.h"
#include "ContentReadWrite.h"
#include "ExplosionGraph.h"
#include "Renderable.h"

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
	void setWindowSize(int newWidth, int newHeight);
	void setMousePos(int x, int y);

	void moveCurrentPart(float inc);

	void updateDistanceBuffer(float inc);
	void updateExplosionTime(float inc);

private:
	GLFWwindow* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;
	ExplosionGraph* graph;

	State state;

	Node* currentNode;
	int mouseX, mouseY;

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

	void _3Dpick();
};
