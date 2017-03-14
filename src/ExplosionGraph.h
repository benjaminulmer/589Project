#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <string>

#include "Renderable.h"

struct Node;

struct Block {
	Block(Node* part, glm::vec3 direction);
	Node* part;
	glm::vec3 direction;
};

struct Node {
	Node();
	Node(Renderable* part, int index);

	Renderable* part;
	int index;

	std::vector<Block> blocking;

	glm::vec3 direction;
	float selfDistance;
	float totalDistance;
};


class ExplosionGraph {

public:
	ExplosionGraph(); // Temporary
	ExplosionGraph(std::vector<Renderable*> parts);

	std::vector<std::vector<Node*>>& getSort();

private:
	std::vector<Node> nodes;
	std::vector<std::list<Node*>> graph;
	std::vector<std::list<Node*>> iGraph;

	std::vector<std::vector<Node*>> topologicalSort;

	void fillDistances();
	void constructInverse();
	void sort();
};

