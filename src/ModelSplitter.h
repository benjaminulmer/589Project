#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <deque>

#include "Renderable.h"

class Renderable;
struct Block;

class ModelSplitter {

public:
	std::vector<Renderable*> split(Renderable* object);
	std::vector<Block*> contactsAndBlocking(std::vector<Renderable*> objects);
	bool lineIntersect(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
	bool pointInTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 p);

};
