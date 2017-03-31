#pragma once

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#undef main
//#include <GLFW/glfw3.h>

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
	void setWindowSize(int newWidth, int newHeight);
	void setMousePos(int x, int y);

	void updateDistanceBuffer(float inc);
	void updateExplosionTime(float inc);

private:
	SDL_Window* window;
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

	float startTime;
};
