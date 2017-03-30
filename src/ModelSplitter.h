#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <deque>

#include "Renderable.h"

struct BlockingPair {
	BlockingPair(unsigned int focusPart, unsigned int otherPart, glm::vec3 direction);
	unsigned int focusPart;
	unsigned int otherPart;
	glm::vec3 direction;
};

class ModelSplitter {

public:
	static std::vector<IndexedLists> split(IndexedLists& object);
	static std::vector<BlockingPair> contactsAndBlocking(std::vector<IndexedLists>& objects);
	static bool lineIntersect(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
	static bool pointInTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 p);

};
