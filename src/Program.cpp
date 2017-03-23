#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	graph = nullptr;

	state = State::NONE;
	level = 0;
	counterS = 0.f;
	timeSPerLevel = 1.f;

	distBuffer = 1.5f;
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
	InputHandler::setUp(camera, renderEngine, this);
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
	//glfwSwapInterval(1); // Vsync on

	glfwSetKeyCallback(window, InputHandler::key);
	glfwSetMouseButtonCallback(window, InputHandler::mouse);
	glfwSetCursorPosCallback(window, InputHandler::motion);
	glfwSetScrollCallback(window, InputHandler::scroll);
	glfwSetWindowSizeCallback(window, InputHandler::reshape);
}

// Loads and initializes all objects that can be viewed
void Program::loadObjects() {
	std::vector<Renderable*> objects = ContentLoading::createRenderables("./models/example.obj");
	//o->textureID = (renderEngine->loadTexture("./textures/cube.png"));

	float i = 0.f;
	float j = 1.f;
	for (Renderable* object : objects) {
		object->colour = glm::vec3(0, i, j);
		i += (1.0 / objects.size());
		j -= (1.0 / objects.size());

		renderEngine->assignBuffers(*object);
	}

	graph = new ExplosionGraph(objects);
}

// Main loop
void Program::mainLoop() {

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (state == State::EXPLODE) {
			explode();
		}
		else if (state == State::COLLAPSE) {
			collapse();
		}

		glfwSetTime(0.);
		renderEngine->render(graph->getSort(), level, counterS / timeSPerLevel, distBuffer);
		renderEngine->pickerRender(graph->getSort(), level, counterS / timeSPerLevel, distBuffer, 0);
		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	glfwDestroyWindow(window);
	glfwTerminate();
}

// Sets state to new state
void Program::setState(State newState) {
	state = newState;
}

// Sets values to animate explosion of model
void Program::explode() {
	counterS += glfwGetTime();

	if (counterS > timeSPerLevel) {

		if (level == graph->getSort().size() - 2) {
			counterS = timeSPerLevel;
			state = State::NONE;
		}
		else {
			counterS = 0.f;
			level++;
		}
	}
}

// Sets values to animate collapse of model
void Program::collapse() {
	counterS -= glfwGetTime();

	if (counterS < 0.f) {

		if (level == 0) {
			counterS = 0.f;
			state = State::NONE;
		}
		else {
			counterS = timeSPerLevel;
			level--;
		}
	}
}
