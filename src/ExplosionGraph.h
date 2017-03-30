#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <string>

#include "Renderable.h"
#include "ModelSplitter.h"

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
	float minSelfDistance;
	float curSelfDistance;
	float totalDistance;

	bool active;

	void move(float dist);
};


class ExplosionGraph {

public:
	ExplosionGraph(std::vector<Renderable*> parts, std::vector<BlockingPair*> blockingPairs);

	void updateDistances();

	std::vector<std::vector<Node*>>& getSort();
	Node* at(int index);

private:
	Node* nodes;
	unsigned int numParts;
	std::vector<std::list<Node*>> graph;
	std::vector<std::list<Node*>> iGraph;

	std::vector<std::vector<Node*>> topologicalSort;

	void constructInverse();
	int sort();

	float getEscapeDistance(Node* node, int sign, char dir, const std::vector<int>& activeSet);
};

