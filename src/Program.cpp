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
	glfwSetErrorCallback(Program::error);
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(width, height, "CPSC589 Project", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Vsync on

	glfwSetKeyCallback(window, InputHandler::key);
	glfwSetMouseButtonCallback(window, InputHandler::mouse);
	glfwSetCursorPosCallback(window, InputHandler::motion);
	glfwSetScrollCallback(window, InputHandler::scroll);
	glfwSetWindowSizeCallback(window, InputHandler::reshape);
}

// Loads and initializes all objects that can be viewed
void Program::loadObjects() {

	// Read in obj
	std::vector<UnpackedLists> split = ContentReadWrite::partsFromObj("./models/test.obj");

	// Create renderables from split object
	std::vector<Renderable*> renderables(split.size());
	for (unsigned int i = 0; i < split.size(); i++) {
		renderables[i] = new Renderable();
		ModelOperations::indexVBO(split[i].verts, split[i].uvs, split[i].normals, renderables[i]->faces, renderables[i]->verts, renderables[i]->uvs, renderables[i]->normals);
	}

	// Set colours and assign buffers
	float i = 0.f;
	float j = 1.f;
	for (Renderable* object : renderables) {
		object->colour = glm::vec3(0, i, j);
		i += (1.0 / renderables.size());
		j -= (1.0 / renderables.size());

		renderEngine->assignBuffers(*object);
	}

	int ver = 0;
	// Compute blocking and create explosion graph
	if (ver == 0) {
		std::cout << "Computing explosion" << std::endl;

		// Compute contacts and blocking
		std::vector<ContactPair> blockings = ModelOperations::contacts(split);
		std::vector<BlockingPair> blocks = ModelOperations::blocking(split);

		std::vector<int> counts(split.size(), 0);
		for (unsigned int i = 0; i < blocks.size(); i++) {
			counts[blocks[i].focusPart]++;
			//if (blocks[i].focusPart == 3)
			printf("part %d, part %d, dir %f, %f, %f\n", blocks[i].focusPart, blocks[i].otherPart, blocks[i].direction.x, blocks[i].direction.y, blocks[i].direction.z);
		}
		std::cout << blocks.size() << std::endl;
		graph = new ExplosionGraph(renderables, blockings);

		rapidjson::Document d = graph->getJSON();
		ContentReadWrite::writeExplosionGraph(d, "./graphs/FixedExample.json");

	}
	// Use file to create explosion graph
	else {
		std::cout << "Using explosion file" << std::endl;

		rapidjson::Document d = ContentReadWrite::readExplosionGraph("./graphs/FixedExample.json");

		if (!d.IsObject()) {
			std::cout << "File is not valid JSON" << std::endl;
			exit(0);
		}
		graph = new ExplosionGraph(renderables, d);
	}
}

// Main loop
void Program::mainLoop() {

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		_3Dpick();
		if (state == State::EXPLODE) {
			explode();
		}
		else if (state == State::COLLAPSE) {
			collapse();
		}

		glfwSetTime(0.);
		renderEngine->render(graph->getSort(), level, counterS / timeSPerLevel, distBuffer);
		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	glfwDestroyWindow(window);
	glfwTerminate();
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
		currentNode->move(-0.3f);
		currentNode->active = false;
	}

	// Get new current node from mouse position (if mouse is on an object)
	if (result != 0) {
		printf("part %f\n", result - 1);
		currentNode = graph->at(result - 1);
		currentNode->move(0.3f);
		currentNode->active = true;
	}
	else {
		currentNode = nullptr;
	}

	graph->updateDistances();
}

// Moves currently selected part provided distance along its explosion direction
void Program::moveCurrentPart(float dist) {
	if (currentNode != nullptr) {
		currentNode->move(dist);
		graph->updateDistances();
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
	if (timeSPerLevel + inc >= 0.24999f) {

		// Update counterS as well to preserve how far things have exploded
		float ratio = counterS / timeSPerLevel;
		timeSPerLevel += inc;
		counterS = timeSPerLevel * ratio;
	}
}
