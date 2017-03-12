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
	loadObject();
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

// Loads and initializes all objects that can be viewed
void Program::loadObject() {
	std::vector<Renderable*> objects;
	objects.push_back(ContentLoading::createRenderable("./models/1.obj"));
	objects.push_back(ContentLoading::createRenderable("./models/2.obj"));
	objects.push_back(ContentLoading::createRenderable("./models/3.obj"));
	objects.push_back(ContentLoading::createRenderable("./models/4.obj"));

	//o->textureID = (renderEngine->loadTexture("./textures/cube.png"));

	graph = ExplosionGraph(objects, true);

	for (Renderable* object : objects) {
		renderEngine->assignBuffers(*object);
	}
}

// Main loop
void Program::mainLoop() {

	std::vector<std::vector<Node*>> sort = graph.getSort();
	int level = sort.size();

	int frame = 0;
	int maxFrame = 120;

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (level >= 0) {

			frame++;
			if (frame > maxFrame) {
				frame = 0;
				level--;
			}
		}

		renderEngine->render(sort, level, (float)frame / (float)maxFrame);
		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	glfwDestroyWindow(window);
	glfwTerminate();
}
