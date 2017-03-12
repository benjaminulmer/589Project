#pragma once

#include <list>

#include "Renderable.h"

struct Node {
	Node();
	Node(Renderable* part, int index);

	Renderable* part;
	int index;

	glm::vec3 direction;
	float selfDistance;
	float totalDistance;
};

class ExplosionGraph {

public:
	ExplosionGraph(); // Temporary
	ExplosionGraph(std::vector<Renderable*> parts, bool test); // Temporary
	ExplosionGraph(std::vector<Renderable*> parts);

	std::vector<std::vector<Node*>>& getSort();

private:
	std::vector<Node> nodes;
	std::vector<std::list<Node*>> graph;
	std::vector<std::list<Node*>> iGraph;

	std::vector<std::vector<Node*>> topologicalSort;

	void fillDistances();
	void constructInverse();
	int sort();
	void fillDistacnes();
};

