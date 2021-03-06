#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
//#include <GLFW/glfw3.h>

#include "Camera.h"
#include "RenderEngine.h"
#include "Program.h"

class Program;

class InputHandler {

public:
	static void setUp(Camera* camera, RenderEngine* renderEngine, Program* program);

	static void pollEvent(SDL_Event& e);

	static void key(SDL_KeyboardEvent& e);
	static void mouse(SDL_MouseButtonEvent& e);
	static void motion(SDL_MouseMotionEvent& e);
	static void scroll(SDL_MouseWheelEvent& e);
	static void reshape(SDL_WindowEvent& e);

private: 
	static Camera* camera;
	static RenderEngine* renderEngine;
	static Program* program;

	static float mouseOldX;
	static float mouseOldY;

	static bool moved;
};
