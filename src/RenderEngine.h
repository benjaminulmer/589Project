#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "ShaderTools.h"
#include "ExplosionGraph.h"
#include "Camera.h"
#include "Texture.h"

#include "lodepng.h"

class RenderEngine {

public:
	RenderEngine(SDL_Window* window, Camera* camera);
	virtual ~RenderEngine();

	void render(const std::vector<std::vector<Node*>>& graph, int level, float perc, float distBuffer);
	void assignBuffers(Renderable& renderable);
	void deleteBuffers(Renderable& renderable);
	unsigned int loadTexture(std::string filename);

	void setWindowSize(int newWidth, int newHeight);
	void updateLightPos(glm::vec3 add);

	int pickerRender(const std::vector<std::vector<Node*>>& graph, int level, float perc, float distBuffer, int x, int y);

private:
	SDL_Window* window;
	int width, height;

	GLuint mainProgram;
	GLuint lightProgram;

	Camera* camera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 lightPos;

	GLuint pickerProgram;
};

