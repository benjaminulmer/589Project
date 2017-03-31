#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	graph = nullptr;

	mouseX = mouseY = 0;
	width = height = 1024;

	state = State::NONE;
	currentNode = nullptr;

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
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	window = SDL_CreateWindow("CPSC589 Project", 200, 0, width, height, SDL_WINDOW_OPENGL);
	if (window == nullptr){
		//TODO: cleanup methods upon exit
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL)
	{
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
	}
	//SDL_GL_MakeCurrent(window, context);

	SDL_GL_SetSwapInterval(1); // Vsync on
}

// Loads and initializes all objects that can be viewed
void Program::loadObjects() {
	std::pair<std::vector<Renderable*>, std::vector<BlockingPair*>> result = ContentLoading::createRenderables("./models/FixedExample.obj");
	std::vector<Renderable*> objects = result.first;
	//o->textureID = (renderEngine->loadTexture("./textures/cube.png"));

	float i = 0.f;
	float j = 1.f;
	for (Renderable* object : objects) {
		object->colour = glm::vec3(0, i, j);
		i += (1.0 / objects.size());
		j -= (1.0 / objects.size());

		renderEngine->assignBuffers(*object);
	}
	graph = new ExplosionGraph(objects, result.second);
}

// Main loop
void Program::mainLoop() {

	startTime = SDL_GetTicks();

	//while(!glfwWindowShouldClose(window)) {
	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		_3Dpick();
		if (state == State::EXPLODE) {
			explode();
		}
		else if (state == State::COLLAPSE) {
			collapse();
		}

		//glfwSetTime(0.);
		startTime = SDL_GetTicks();
		renderEngine->render(graph->getSort(), level, counterS / timeSPerLevel, distBuffer);

		SDL_GL_SwapWindow(window);
	}

	// Clean up, program needs to exit
	SDL_Quit();
}

// Sets values to animate explosion of model
void Program::explode() {
	counterS += (float)(SDL_GetTicks() - startTime) / 1000;

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
	counterS -= (float)(SDL_GetTicks() - startTime) / 1000;

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

// Sets state to new state
void Program::setState(State newState) {
	state = newState;
}

// Sets new width and height
void Program::setWindowSize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	renderEngine->setWindowSize(newWidth, newHeight);
}

// Update position of mouse. [0, 0] is bottom left corner; [1, 1] is top right
void Program::setMousePos(int x, int y) {
	mouseX = x;
	mouseY = y;
}

// Finds objects mouse is over and makes it current
void Program::_3Dpick() {
	float result = renderEngine->pickerRender(graph->getSort(), level, counterS / timeSPerLevel, distBuffer, mouseX, mouseY);

	// Reset current active node (if there is one)
	if (currentNode != nullptr) {
		currentNode->active = false;
	}

	// Get new current node from mouse position (if mouse is on an object)
	if (result != 0) {
		currentNode = graph->at(result - 1);
		currentNode->active = true;
	}
	else {
		currentNode = nullptr;
	}
}

// Updates buffer between objects when exploding
void Program::updateDistanceBuffer(float inc) {
	if (distBuffer + inc >= 0.99999f) {
		distBuffer += inc;
	}
}

// Updates buffer between objects when exploding
void Program::updateExplosionTime(float inc) {
	if (timeSPerLevel + inc >= 0.19999f) {
		timeSPerLevel += inc;
	}
}
