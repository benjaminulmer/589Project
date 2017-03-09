#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "ShaderTools.h"
#include "Renderable.h"
#include "Camera.h"
#include "Texture.h"

#include "lodepng.h"

class RenderEngine {

public:
	RenderEngine(GLFWwindow* window, Camera* camera);
	virtual ~RenderEngine();

	void render(const Renderable& renderable);
	void assignBuffers(Renderable& renderable);
	unsigned int loadTexture(std::string filename);

	void setWindowSize(int width, int height);
	void updateLightPos(glm::vec3 add);

private:
	GLFWwindow* window;

	GLuint mainProgram;
	GLuint lightProgram;

	Camera* camera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 lightPos;

	void renderLight();
};

