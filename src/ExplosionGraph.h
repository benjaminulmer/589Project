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
	ExplosionGraph(); // Temporary

	ExplosionGraph(std::vector<Renderable*> parts);

	std::vector<std::vector<Node>>& getSort();

private:
	std::vector<std::list<Node>> graph;
	std::vector<std::list<Node>> iGraph;

	std::vector<std::vector<Node>> topologicalSort;
	bool isSorted;

	void fillDistacnes();
};

