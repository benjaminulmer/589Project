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

	std::vector<Block> blocked;

	glm::vec3 direction;
	float selfDistance;
	float totalDistance;

	float id;
};


class ExplosionGraph {

public:
	ExplosionGraph(std::vector<Renderable*> parts, bool test); // Temporary
	ExplosionGraph(std::vector<Renderable*> parts);

	std::vector<std::vector<Node*>>& getSort();

private:
	Node* nodes;
	unsigned int numParts;
	std::vector<std::list<Node*>> graph;
	std::vector<std::list<Node*>> iGraph;

	std::vector<std::vector<Node*>> topologicalSort;

	void fillDistances();
	void constructInverse();
	int sort();

	void hardCodedBlocking();
	float getEscapeDistance(Node* node, int sign, char dir, const std::vector<int>& activeSet);
};

