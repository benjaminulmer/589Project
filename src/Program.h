#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main
//#include <GLFW/glfw3.h>

#include "InputHandler.h"
#include "RenderEngine.h"
#include "Camera.h"
#include "ContentReadWrite.h"
#include "ExplosionGraph.h"
#include "Renderable.h"

enum class AniamtionState {
	NONE,
	EXPLODE,
	COLLAPSE
};

class Program {

public:
	Program();
	Program(char* file);
	Program(char* file, char* explosionfile);
	virtual ~Program();

	void start();

	void setState(AniamtionState newState);
	void setWindowSize(int newWidth, int newHeight);
	void setMousePos(int x, int y);

	void _3Dpick(bool select);
	void moveCurrentPart(float inc);

	void updateDistanceBuffer(float inc);
	void updateExplosionTime(float inc);

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;
	ExplosionGraph* graph;

	AniamtionState state;

	Node* highlightNode;
	Node* selectedNode;
	int mouseX, mouseY;

	unsigned int level;
	float counterS;
	float timeSPerLevel;
	float distBuffer;

	void setupWindow();
	void loadObjects();
	void mainLoop();

	void explode();
	void collapse();

	float startTime;

	char* filename;
	char* explosionFilename;
};
