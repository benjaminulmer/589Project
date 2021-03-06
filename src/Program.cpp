#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	graph = nullptr;

	mouseX = mouseY = 0;
	width = height = 1024;

	state = AniamtionState::NONE;
	highlightNode = nullptr;
	selectedNode = nullptr;

	level = 0;
	counterS = 0.f;
	timeSPerLevel = 1.f;

	distBuffer = 1.5f;
	startTime =  0;

	filename = "./models/mechanical.obj";
	explosionFilename = "./graphs/mechanical.json";
}

Program::Program(char* file) {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	graph = nullptr;

	mouseX = mouseY = 0;
	width = height = 1024;

	state = AniamtionState::NONE;
	highlightNode = nullptr;
	selectedNode = nullptr;

	level = 0;
	counterS = 0.f;
	timeSPerLevel = 1.f;

	distBuffer = 1.5f;
	startTime = 0;

	filename = file;
	explosionFilename = "";
}

Program::Program(char* file, char* explosionFile) {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	graph = nullptr;

	mouseX = mouseY = 0;
	width = height = 1024;

	state = AniamtionState::NONE;
	highlightNode = nullptr;
	selectedNode = nullptr;

	level = 0;
	counterS = 0.f;
	timeSPerLevel = 1.f;

	distBuffer = 1.5f;
	startTime = 0;

	filename = file;
	explosionFilename = explosionFile;
}

// Called to start the program. Conducts set up then enters the main loop
void Program::start() {
	loadObjects();

	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	camera = new Camera();
	renderEngine = new RenderEngine(window, camera);
	InputHandler::setUp(camera, renderEngine, this);

	assignBuffers();
	mainLoop();
}

// Creates SDL window for the program and sets callbacks for input
void Program::setupWindow() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("CPSC589 Project", 10, 30, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

	SDL_GL_SetSwapInterval(1); // Vsync on
}

// Loads and initializes all objects that can be viewed
void Program::loadObjects() {

	// Read in obj
	std::vector<UnpackedLists> split = ContentReadWrite::partsFromObj(filename);

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
	}

	// Compute blocking and create explosion graph
	if (explosionFilename.length() == 0) {
		std::cout << "Computing explosion" << std::endl;

		// Compute blockings
		std::vector<BlockingPair> blocks = ModelOperations::blocking(split);
		graph = new ExplosionGraph(renderables, blocks);

		rapidjson::Document d = graph->getJSON();

		// Parse the model's filename to get the proper name for the JSON file
		std::stringstream ss;
		ss.str(filename);
		std::string item;
		std::vector<std::string> parseFilename;
		while (std::getline(ss, item, '/')) {
			parseFilename.push_back(item);
		}
		std::cout << parseFilename[parseFilename.size() - 1] << std::endl;
		std::string tail = parseFilename[parseFilename.size() - 1];
		parseFilename.clear();
		std::stringstream ss2;
		ss2.str(tail);
		while (std::getline(ss2, item, '.')) {
			parseFilename.push_back(item);
		}

		// Write to JSON file
		ContentReadWrite::writeExplosionGraph(d, "./graphs/" + parseFilename[0] + ".json");
	}
	// Use file to create explosion graph
	else {
		std::cout << "Using explosion file " << explosionFilename << std::endl;

		rapidjson::Document d = ContentReadWrite::readExplosionGraph(explosionFilename);

		if (!d.IsObject()) {
			std::cout << "Could not read explosion file" << std::endl;
			exit(0);
		}
		graph = new ExplosionGraph(renderables, d);
	}
}

// Assigned buffers for all objects for rendering
void Program::assignBuffers() {
	std::vector<std::vector<Node*>>& sort = graph->getSort();
	for (std::vector<Node*> l : sort) {
		for (Node* node : l) {
			Renderable* renderable = node->part;
			renderEngine->assignBuffers(*renderable);
			renderEngine->setBufferData(*renderable);
		}
	}
}

// Main loop
void Program::mainLoop() {

	startTime = SDL_GetTicks();

	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		_3Dpick(false);
		if (state == AniamtionState::EXPLODE) {
			explode();
		}
		else if (state == AniamtionState::COLLAPSE) {
			collapse();
		}

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
			state = AniamtionState::NONE;
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
			state = AniamtionState::NONE;
		}
		else {
			counterS = timeSPerLevel;
			level--;
		}
	}
}

// Sets state to new state
void Program::setState(AniamtionState newState) {
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

// Finds objects mouse is over and sets it state to the provided
void Program::_3Dpick(bool select) {
	float result = renderEngine->pickerRender(graph->getSort(), level, counterS / timeSPerLevel, distBuffer, mouseX, mouseY);

	// Reset current active node (if there is one)
	if (highlightNode != nullptr && !select) {
		highlightNode->highlighted = false;
	}
	else if  (selectedNode != nullptr && select) {
		selectedNode->selected = false;
	}

	// Get new current node from mouse position (if mouse is on an object)
	if (result != 0) {

		if (!select) {
			highlightNode = graph->at(result - 1);
			highlightNode->highlighted = true;
		}
		else {
			selectedNode = graph->at(result - 1);
			selectedNode->selected = true;
		}
	}
	else {
		if (!select) {
			highlightNode = nullptr;
		}
		else {
			selectedNode = nullptr;
		}
	}

	graph->updateDistances();
}

// Moves currently selected part provided distance along its explosion direction
void Program::moveCurrentPart(float dist) {
	if (selectedNode != nullptr) {
		selectedNode->move(dist);
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
