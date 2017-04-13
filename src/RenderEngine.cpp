#include "RenderEngine.h"

RenderEngine::RenderEngine(SDL_Window* window, Camera* camera) :
	window(window), camera(camera) {

	SDL_GetWindowSize(window, &width, &height);

	mainProgram = ShaderTools::compileShaders("./shaders/mesh.vert", "./shaders/mesh.frag");
	pickerProgram = ShaderTools::compileShaders("./shaders/picker.vert", "./shaders/picker.frag");

	lightPos = glm::vec3(0.0, 2.0, 0.0);
	projection = glm::perspective(45.0f, (float)width/height, 0.01f, 100.0f);

	// Default openGL state
	// If you change state you must change back to default after
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE);
	glPointSize(30.0f);
	glClearColor(0.3, 0.3, 0.4, 0.0);
}

RenderEngine::~RenderEngine() {
	// nothing to do here, program will clean up window pointer
}

// Called to render the active object. RenderEngine stores all information about how to render
void RenderEngine::render(const std::vector<std::vector<Node*>>& graph, int level, float perc, float distBuffer) {
	glUseProgram(mainProgram);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.3, 0.3, 0.4, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	level = graph.size() - level - 1;

	int i = 0;
	for (std::vector<Node*> l : graph) {
		for (Node* node : l) {
			Renderable* renderable = node->part;
			glBindVertexArray(renderable->vao);

			// If the object has no image texture switch to attribute only mode
			Texture::bind2DTexture(mainProgram, renderable->textureID, "image");

			view = camera->getLookAt();
			glm::mat4 model;

			// Determine how far to move object
			glm::vec3 dir = node->direction;
			if (i > level) {
				model = glm::translate(dir * node->totalDistance * distBuffer);
			}
			else if (i == level) {
				model = glm::translate(dir * node->totalDistance * perc * distBuffer);
			}
			else {
				model = glm::mat4();
			}

			glm::mat4 modelView = view * model;

			// Uniforms
			glUniformMatrix4fv(glGetUniformLocation(mainProgram, "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));
			glUniformMatrix4fv(glGetUniformLocation(mainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniform3fv(glGetUniformLocation(mainProgram, "lightPos"), 1, glm::value_ptr(lightPos));

			glm::vec3 colour;
			if (node->selected) {
				colour = glm::vec3(1.f, 1.f, 1.f);
			}
			else if (node->highlighted) {
				colour = renderable->colour * 1.3f;
			}
			else {
				colour = renderable->colour;
			}

			glUniform3fv(glGetUniformLocation(mainProgram, "objColour"), 1, glm::value_ptr(colour));
			glUniform1i(glGetUniformLocation(mainProgram, "hasTexture"), (renderable->textureID > 0 ? 1 : 0));

			glDrawElements(GL_TRIANGLES, renderable->faces.size(), GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
			Texture::unbind2DTexture();
		}
		i++;
	}
}

int RenderEngine::pickerRender(const std::vector<std::vector<Node*>>& graph, int level, float perc, float distBuffer, int x, int y) {

	// Create framebuffer with texture to render IDs to
	GLuint texID;
	GLuint frameBuffer;
	GLuint depthBuffer;

	// Create and bind framebuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Create texture and set texture parameters
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);

	// Create depth buffer
	glGenRenderbuffersEXT(1, &depthBuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);

	// Attack texture and depth buffer to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texID, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);


	// Render IDs to textures in framebuffer
	glUseProgram(pickerProgram);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	level = graph.size() - level - 1;

	int i = 0;
	for (std::vector<Node*> l : graph) {
		for (Node* node : l) {
			Renderable* renderable = node->part;
			glBindVertexArray(renderable->vao);

			// If the object has no image texture switch to attribute only mode
			Texture::bind2DTexture(pickerProgram, renderable->textureID, "image");

			view = camera->getLookAt();
			glm::mat4 model;

			// Determine how far to move object
			glm::vec3 dir = node->direction;
			if (i > level) {
				model = glm::translate(dir * node->totalDistance * distBuffer);
			}
			else if (i == level) {
				model = glm::translate(dir * node->totalDistance * perc * distBuffer);
			}
			else {
				model = glm::mat4();
			}

			glm::mat4 modelView = view * model;

			// Uniforms
			glUniformMatrix4fv(glGetUniformLocation(pickerProgram, "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));
			glUniformMatrix4fv(glGetUniformLocation(pickerProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniform1i(glGetUniformLocation(pickerProgram, "id"), node->index + 1);

			glDrawElements(GL_TRIANGLES, renderable->faces.size(), GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
			Texture::unbind2DTexture();
		}
		i++;
	}

	// Get render data back to CPU
	std::vector<int> pixelsVec(width*height);
	int* pixels = pixelsVec.data();

	glBindTexture(GL_TEXTURE_2D, texID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, pixels);

	glDeleteTextures(1, &texID);
	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteFramebuffers(1, &frameBuffer);

	// Return ID at pixel of interest
	return *(pixels + width*y + x);
}

// Assigns and binds buffers for a renderable (sends it to the GPU)
void RenderEngine::assignBuffers(Renderable& renderable) {
	std::vector<glm::vec3>& vertices = renderable.verts;
	std::vector<glm::vec3>& normals = renderable.normals;
	std::vector<glm::vec2>& uvs = renderable.uvs;
	std::vector<GLushort>& faces = renderable.faces;

	// Bind attribute array for triangles
	glGenVertexArrays(1, &renderable.vao);
	glBindVertexArray(renderable.vao);

	// Vertex buffer
	glGenBuffers(1, &renderable.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Normal buffer
	glGenBuffers(1, &renderable.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// UV buffer
	glGenBuffers(1, &renderable.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uvs.size(), uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	// Face buffer
	glGenBuffers(1, &renderable.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*faces.size(), faces.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

// Deletes buffers
void RenderEngine::deleteBuffers(Renderable& renderable) {
	glDeleteBuffers(1, &renderable.vertexBuffer);
	glDeleteBuffers(1, &renderable.normalBuffer);
	glDeleteBuffers(1, &renderable.indexBuffer);

	glDeleteVertexArrays(1, &renderable.vao);
}

// Creates a 2D texture
unsigned int RenderEngine::loadTexture(std::string filename) {
	//reading model texture image
	std::vector<unsigned char> _image;
	unsigned int _imageWidth, _imageHeight;

	unsigned int error = lodepng::decode(_image, _imageWidth, _imageHeight, filename.c_str());
	if (error)
	{
		std::cout << "reading error" << error << ":" << lodepng_error_text(error) << std::endl;
	}

	unsigned int id = Texture::create2DTexture(_image, _imageWidth, _imageHeight);
	return id;
}

// Sets projection and viewport for new width and height
void RenderEngine::setWindowSize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	projection = glm::perspective(45.0f, (float)width/height, 0.01f, 100.0f);
	glViewport(0, 0, width, height);
}

// Updates lightPos by specified value
void RenderEngine::updateLightPos(glm::vec3 add) {
	lightPos += add;
}
