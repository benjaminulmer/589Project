#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
}

Program::~Program() {
	// nothing to do here, end of mainLoop performs clean up
}

// Error callback for glfw errors
void Program::error(int error, const char* description) {
	std::cerr << description << std::endl;
}

// Called to start the program. Conducts set up then enters the main loop
void Program::start() {
	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	camera = new Camera();
	renderEngine = new RenderEngine(window, camera);
	InputHandler::setUp(camera, renderEngine);
	loadObjects();
	mainLoop();
}

// Creates GLFW window for the program and sets callbacks for input
void Program::setupWindow() {
	glfwSetErrorCallback(Program::error);
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(1024, 1024, "CPSC589 Project", NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, InputHandler::key);
	glfwSetMouseButtonCallback(window, InputHandler::mouse);
	glfwSetCursorPosCallback(window, InputHandler::motion);
	glfwSetScrollCallback(window, InputHandler::scroll);
	glfwSetWindowSizeCallback(window, InputHandler::reshape);
}

void Program::loadObjects() {
	Renderable* object = ContentLoading::createRenderable("./models/example/a.obj");
	renderEngine->assignBuffers(*object);
	object->colour = glm::vec4(0.f, 1.f, 0.f, 1.f);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/example/b.obj");
	object->colour = glm::vec4(0.f, 0.9f, 0.2f, 1.f);
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/example/c.obj");
	object->colour = glm::vec4(0.f, 0.8f, 0.4f, 1.f);
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/example/d.obj");
	object->colour = glm::vec4(0.f, 0.7f, 0.5f, 1.f);
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/example/e.obj");
	object->colour = glm::vec4(0.f, 0.6f, 0.5f, 1.f);
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/example/f.obj");
	object->colour = glm::vec4(0.f, 0.4f, 0.6f, 1.f);
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/example/g.obj");
	object->colour = glm::vec4(0.f, 0.2f, 0.8f, 1.f);
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/example/h.obj");
	object->colour = glm::vec4(0.f, 0.f, 1.f, 1.f);
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	/*Renderable* object = ContentLoading::createRenderable("./models/lock.obj");
	renderEngine->assignBuffers(*object);
	objects.push_back(object);

	object = ContentLoading::createRenderable("./models/cube.obj");
	//o->textureID = (renderEngine->loadTexture("./textures/cube.png"));
	renderEngine->assignBuffers(*object);
	objects.push_back(object);*/
}

// Main loop
void Program::mainLoop() {

	ExplosionGraph eg = ExplosionGraph(objects);

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Will probably be changed to take topological sort of the graph instead of Renderable*
		// Also frame number/graph depth for animation
		renderEngine->render(objects);
		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	glfwDestroyWindow(window);
	glfwTerminate();
}
