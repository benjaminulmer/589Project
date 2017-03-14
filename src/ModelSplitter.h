#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <deque>

#include "Renderable.h"

class Renderable;

class ModelSplitter {

public:
	std::vector<Renderable*> split(Renderable* object);

};
