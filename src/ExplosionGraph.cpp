#include "ExplosionGraph.h"

#include <iostream>

// Constructor for a block, always needs part and direction
Block::Block(Node* part, glm::vec3 direction) : part(part), direction(direction) {}

// Default constructor. Zeros everything
Node::Node() : part(0), index(0), minSelfDistance(0.0f), curSelfDistance(0.0f), totalDistance(0.0f),
		highlighted(false), selected(false) {}

// Node for part with given index
Node::Node(Renderable* part, int index) : part(part), index(index), minSelfDistance(0.0f), curSelfDistance(0.0f), totalDistance(0.0f),
		highlighted(false), selected(false){}

// Moves node distance along its explosion direction within valid ranges
void Node::move(float dist) {
	if (minSelfDistance == 0.0f) {
		return;
	}

	curSelfDistance += dist;
	if (curSelfDistance < minSelfDistance) {
		curSelfDistance = minSelfDistance;
	}
}

// Creates explosion graph from blocking constraints
ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts, std::vector<BlockingPair> blockingPairs) {

	// Number of nodes in graph is number of parts in model
	numParts = parts.size();
	graph = std::vector<std::list<Node*>>(numParts);
	iGraph = std::vector<std::list<Node*>>(numParts);

	// Fill list of nodes
	nodes = new Node[numParts];
	std::vector<int> activeSet;
	for (unsigned int i = 0; i < numParts; i++) {
		nodes[i] = Node(parts[i], i);
		graph[i].push_back(&nodes[i]);
		activeSet.push_back(i);
	}

	// Fill blocking data from provided blocking pairs
	for (BlockingPair& block : blockingPairs) {
		nodes[block.focusPart].blocked.push_back(Block(&nodes[block.otherPart], block.direction));
	}

	// Construct the explosion graph
	while (activeSet.size() > 0) {
		std::cout << "set: ";
		for (int active : activeSet) {
			std::cout << active << " ";
		}
		std::cout << std::endl;

		// Create subset of unblocked pieces
		std::vector<int> unblocked;
		std::vector<int> moved;
		for (unsigned int j = 0; j < activeSet.size(); j++) {
			// setup booleans for the six directions
			bool xPlus = true;
			bool xMinus = true;
			bool zPlus = true;
			bool zMinus = true;
			bool yPlus = true;
			bool yMinus = true;
			for (Block block : nodes[activeSet[j]].blocked) {

				// if blocking part is in the active set
				if (std::find(activeSet.begin(), activeSet.end(), block.part->index) != activeSet.end()) {
					if (block.direction.x == 1.f) xPlus = false;
					else if (block.direction.x == -1.f) xMinus = false;
					else if (block.direction.z == 1.f) zPlus = false;
					else if (block.direction.z == -1.f) zMinus = false;
					else if (block.direction.y == 1.f) yPlus = false;
					else if (block.direction.y == -1.f)  yMinus = false;
				}
			}
			if (((xPlus || xMinus) || (yPlus || yMinus)) || (zPlus || zMinus)) {
			/*if ((xPlus || xMinus) || (zPlus || zMinus))*/ unblocked.push_back(activeSet[j]);
			}
		}

		std::vector<int> movedThisIteration;
		for (unsigned int m = 0; m < unblocked.size(); m++) {
			std::vector<int> blocking;
			glm::vec3 unblockDirection;
			float minDistance = 10000.f;

			bool xPlus = true;
			bool xMinus = true;
			bool zPlus = true;
			bool zMinus = true;
			bool yPlus = true;
			bool yMinus = true;
			for (Block block : nodes[unblocked[m]].blocked) {
				// if blocking part is in the active set
				if (std::find(activeSet.begin(), activeSet.end(), block.part->index) != activeSet.end()) {
					blocking.push_back(block.part->index);
					if (block.direction.x == 1.f) xPlus = false;
					else if (block.direction.x == -1.f) xMinus = false;
					else if (block.direction.z == 1.f) zPlus = false;
					else if (block.direction.z == -1.f) zMinus = false;
					else if (block.direction.y == 1.f) yPlus = false;
					else if (block.direction.y == -1.f) yMinus = false;
				}
			}
			Node* curNode = &nodes[unblocked[m]];

			// find shortest distance to escape bounding box of active parts this part is in contact with
			if (xPlus) {
				float xplusDist = getEscapeDistance(curNode, 1, 'x', activeSet);
				if (xplusDist < minDistance) {
					unblockDirection = glm::vec3(1.f, 0.f, 0.f);
					minDistance = xplusDist;
				}
			}
			if (xMinus) {
				float xminDist = getEscapeDistance(curNode, -1, 'x', activeSet);
				if (xminDist < minDistance) {
					unblockDirection = glm::vec3(-1.f, 0.f, 0.f);
					minDistance = xminDist;
				}
			}
			if (zPlus) {
				float zplusDist = getEscapeDistance(curNode, 1, 'z', activeSet);
				if (zplusDist < minDistance) {
					unblockDirection = glm::vec3(0.f, 0.f, 1.f);
					minDistance = zplusDist;
				}
			}
			if (zMinus) {
				float zminDist = getEscapeDistance(curNode, -1, 'z', activeSet);
				if (zminDist < minDistance) {
					unblockDirection = glm::vec3(0.f, 0.f, -1.f);
					minDistance = zminDist;
				}
			}
			if (yPlus) {
				float yplusDist = getEscapeDistance(curNode, 1, 'y', activeSet);
				if (yplusDist < minDistance) {
					unblockDirection = glm::vec3(0.f, 1.f, 0.f);
					minDistance = yplusDist;
				}
			}

			if (yMinus) {
				float yminDist = getEscapeDistance(curNode, -1, 'y', activeSet);
				if (yminDist < minDistance) {
					unblockDirection = glm::vec3(0.f, -1.f, 0.f);
					minDistance = yminDist;
				}
			}

			nodes[unblocked[m]].direction = unblockDirection;
			nodes[unblocked[m]].minSelfDistance = minDistance;
			nodes[unblocked[m]].curSelfDistance = minDistance;

			//activeSet.erase(std::find(activeSet.begin(), activeSet.end(), unblocked[m]));
			movedThisIteration.push_back(unblocked[m]);
		}
		for (int p : movedThisIteration) {
			if (activeSet.size() == 1) {
				for (int i : movedThisIteration) {
					if (i != p) {
						graph[p].push_back(&nodes[i]);

					}
				}
				for (int i : moved) {
					graph[p].push_back(&nodes[i]);
				}
			}
			activeSet.erase(std::find(activeSet.begin(), activeSet.end(), p));
			for (int m : moved) {

				for (Block b : nodes[p].blocked) {

					if ((b.direction == nodes[p].direction) && b.part == &nodes[m]) {
						graph[p].push_back(&nodes[m]);
					}
				}
			}
		}
		for (int i : movedThisIteration) {
			moved.push_back(i);
		}
		movedThisIteration.clear();
	}

	// Construct other graph info
	constructInverse();
	if (sort() == -1) {
		std::cout << "Error, graph contains cycle(s)" << std::endl;
	}
	updateDistances();
}

// Creates explosion graph input file
ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts, rapidjson::Document& d) {

	// Number of nodes in graph is number of parts in model
	numParts = parts.size();
	graph = std::vector<std::list<Node*>>(numParts);
	iGraph = std::vector<std::list<Node*>>(numParts);

	// Fill list of nodes
	nodes = new Node[numParts];
	for (unsigned int i = 0; i < numParts; i++) {
		nodes[i] = Node(parts[i], i);
	}

	// Fill in node information
	for (rapidjson::SizeType i = 0; i < d["nodes"].Size(); i++) {
		rapidjson::Value& node = d["nodes"][i];

		nodes[i].direction.x = (float)node["x"].GetDouble();
		nodes[i].direction.y = (float)node["y"].GetDouble();
		nodes[i].direction.z = (float)node["z"].GetDouble();

		nodes[i].minSelfDistance = (float)node["distance"].GetDouble();
		nodes[i].curSelfDistance = (float)node["distance"].GetDouble();
	}

	// Populate graph
	for (rapidjson::SizeType i = 0; i < d["graph"].Size(); i++) {
		rapidjson::Value& array = d["graph"][i];

		for (rapidjson::SizeType j = 0; j < array.Size(); j++) {

			int val = array[j].GetInt();
			graph[i].push_back(&nodes[val]);
		}
	}

	// Construct other graph info
	constructInverse();
	if (sort() == -1) {
		std::cout << "Error, graph contains cycle(s)" << std::endl;
	}
	updateDistances();
}

// Finds escape distance for a node in given direction (sign combined with axis)
float ExplosionGraph::getEscapeDistance(Node* node, int sign, char dir, const std::vector<int>& activeSet) {

	float toReturn = 0.f;
	glm::vec3 partPos = node->part->getPosition();
	glm::vec3 partDimensions = node->part->getDimensions();

	for (Block block : node->blocked) {
		if (std::find(activeSet.begin(), activeSet.end(), block.part->index) != activeSet.end()) {
			glm::vec3 blockDimensions = block.part->part->getDimensions();
			glm::vec3 blockPos = block.part->part->getPosition();

			float newPos;
			float newDist;
			if (dir == 'x') {
				newPos = blockPos.x + sign * ((0.5 * partDimensions.x) + (0.5 * blockDimensions.x));
				newDist = abs(newPos - partPos.x);
			}
			else if (dir == 'y') {
				newPos = blockPos.y + sign * ((0.5 * partDimensions.y) + (0.5 * blockDimensions.y));
				newDist = abs(newPos - partPos.y);
			}
			else {
				newPos = blockPos.z + sign * ((0.5 * partDimensions.z) + (0.5 * blockDimensions.z));
				newDist = abs(newPos - partPos.z);
			}

			if (newDist > toReturn) toReturn = newDist;
		}
	}
	return toReturn;
}

// Constructs inverse graph from normal graph
void ExplosionGraph::constructInverse() {

	// Add self for each edge list
	for (unsigned int i = 0; i < numParts; i++) {
		iGraph[i].push_back(&nodes[i]);
	}

	// Construct inverse
	for (unsigned int i = 0; i < numParts; i++) {
		int j = 0;
		for (Node* neighbour : graph[i]) {

			// Don't add nodes self to inverse, already did that
			if (j > 0) {
				iGraph[neighbour->index].push_back(&nodes[i]);
			}

			j++;
		}
	}
}

// Topologically sorts the graph
int ExplosionGraph::sort() {

	// Copy so we don't ruin the actual graph
	std::vector<std::list<Node*>> iGraphMain = iGraph;

	std::vector<Node*> queue;

	// Continue until all nodes have been sorted
	unsigned int numAdded = 0;
	while (numAdded < numParts) {
		bool found = false;

		// Another copy so all nodes of can be found for each level
		std::vector<std::list<Node*>> iGraphC = iGraphMain;

		// Loop over all nodes in graph
		for (unsigned int i = 0; i < iGraphC.size(); i++) {

			// One parent (only self) means no incoming edges. 0 means it has been removed
			if (iGraphC[i].size() == 1) {
				Node* self = iGraphC[i].front();

				found = true;
				queue.push_back(self);
				numAdded++;

				// Remove outgoing edges from graph
				for (Node* child : graph[i]) {
					iGraphMain[child->index].remove(self);
				}
			}
		}
		// If no node found then there is a cycle, return error
		if (!found){
			return -1;
		}

		// Add found nodes to current level then clear the queue
		topologicalSort.push_back(queue);
		queue.clear();
	}

	return 0;
}

// Fills in total distance of each node in graph based off of parent distances
void ExplosionGraph::updateDistances() {

	for (unsigned int i = 0; i < numParts; i++) {
		nodes[i].totalDistance = 0.0f;
	}

	// Loop over all nodes by level in the topological sort
	for (std::vector<Node*> level : topologicalSort) {
		for (Node* node : level) {

			// Check all parents for the one with the largest total distance in same direction as self
			float max = 0.0f;
			for (Node* parent : iGraph[node->index]) {
				if (parent->direction == node->direction && parent->totalDistance > max) {
					max = parent->totalDistance;
				}
			}

			// Update total distance by the largest found
			node->totalDistance = node->curSelfDistance + max;
		}
	}
}

// Returns topologically sorted graph. Nodes of same depth are grouped together inside inner vector
std::vector<std::vector<Node*>>& ExplosionGraph::getSort() {
	return topologicalSort;
}

// Returns pointer to node at index or nullptr if index is out of bounds
Node* ExplosionGraph::at(int index) {
	if (index < 0 || (unsigned int)index >= numParts) {
		return nullptr;
	}
	else {
		return &nodes[index];
	}
}

// Returns JSON representation of explosion graph
rapidjson::Document ExplosionGraph::getJSON() {
	rapidjson::Document d;
	d.SetObject();

	rapidjson::Document::AllocatorType& alloc = d.GetAllocator();

	// Create array of nodes
	rapidjson::Value jNodes;
	jNodes.SetArray();

	// Make entry for each node
	for (unsigned int i = 0; i < numParts; i++) {
		rapidjson::Value node;
		node.SetObject();
		node.AddMember("x", nodes[i].direction.x, alloc);
		node.AddMember("y", nodes[i].direction.y, alloc);
		node.AddMember("z", nodes[i].direction.z, alloc);
		node.AddMember("distance", nodes[i].minSelfDistance, alloc);

		jNodes.PushBack(node, alloc);
	}
	d.AddMember("nodes", jNodes, alloc);

	// Create array for graph
	rapidjson::Value jGraph;
	jGraph.SetArray();

	// Create inner array for each node in graph
	for (std::list<Node*> l : graph) {
		rapidjson::Value array;
		array.SetArray();

		// Fill inner array
		for (Node* n : l) {
			array.PushBack(n->index, alloc);
		}
		jGraph.PushBack(array, alloc);
	}
	d.AddMember("graph", jGraph, alloc);

	return d;
}
