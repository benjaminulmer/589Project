#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <rapidjson/document.h>

#include "ModelOperations.h"
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
	float minSelfDistance;
	float curSelfDistance;
	float totalDistance;

	bool highlighted;
	bool selected;

	void move(float dist);
};


class ExplosionGraph {

public:
	ExplosionGraph(std::vector<Renderable*> parts, std::vector<ContactPair> blockingPairs);
	ExplosionGraph(std::vector<Renderable*> parts, rapidjson::Document& d);

	void updateDistances();

	std::vector<std::vector<Node*>>& getSort();
	Node* at(int index);

	rapidjson::Document getJSON();

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

