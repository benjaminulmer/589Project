#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
	RenderEngine(GLFWwindow* window, Camera* camera);
	virtual ~RenderEngine();

	void render(const std::vector<std::vector<Node*>>& graph, int level, float perc, float distBuffer);
	void assignBuffers(Renderable& renderable);
	void deleteBuffers(Renderable& renderable);
	unsigned int loadTexture(std::string filename);

	void setWindowSize(int width, int height);
	void updateLightPos(glm::vec3 add);

	int pickerRender(const std::vector<std::vector<Node*>>& graph, int level, float perc, float distBuffer, int x, int y);

private:
	GLFWwindow* window;
	int width, height;

	GLuint mainProgram;
	GLuint lightProgram;

	Camera* camera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 lightPos;

	GLuint pickerProgram;

	void renderLight();
};

