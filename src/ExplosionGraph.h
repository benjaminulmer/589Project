#pragma once

#include <list>

#include "Renderable.h"

struct Node {
	Renderable* part;

	glm::vec3 direction;
	float selfDistance;
	float totalDistance;
};

class ExplosionGraph {

public:
	ExplosionGraph(std::vector<Renderable> parts);

private:


};

