#include "ExplosionGraph.h"

#include <iostream>

// Default constructor. Zeros everything
Node::Node() : part(0), index(0), selfDistance(0.0f), totalDistance(0.0f), active(false) {}

// Node for part with given index
Node::Node(Renderable* part, int index) : part(part), index(index), selfDistance(0.0f), totalDistance(0.0f), active(false) {}

ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts, bool test) {

	// Number of nodes in graph is number of parts in model
	numParts = parts.size();
	graph = std::vector<std::list<Node*>>(numParts);
	iGraph = std::vector<std::list<Node*>>(numParts);

	// Fill list of nodes
	nodes = new Node[numParts];
	for (unsigned int i = 0; i < numParts; i++) {
		nodes[i] = Node(parts[i], i);
		graph[i].push_back(&nodes[i]);
	}

	graph[0].push_back(&nodes[2]); graph[0].push_back(&nodes[3]);
	graph[1].push_back(&nodes[0]);

	nodes[0].direction = glm::vec3(0.0f, -1.0f, 0.0f);
	nodes[1].direction = glm::vec3(0.0f, -1.0f, 0.0f);
	nodes[2].direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	nodes[3].direction = glm::vec3(1.0f, 0.0f, 0.0f);

	nodes[0].selfDistance = 0.0f;
	nodes[1].selfDistance = 2.0f;
	nodes[2].selfDistance = 1.0f;
	nodes[3].selfDistance = 1.0f;

	constructInverse();
	if (sort() == -1) {
		std::cout << "Error, graph contains cycle(s)" << std::endl;
	}
	fillDistances();
}

// Constructor for a block, always needs part and direction
Block::Block(Node* part, glm::vec3 direction) : part(part), direction(direction) {}


// Creates explosion graph for provided parts
ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts) {

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
	// **** to add an edge: graph[parentIndex].push_back(&nodes[childIndex]) ***** //

	// Compute contacts and blocking (John)
	hardCodedBlocking();


	// Construct graph from contacts and blocking (Mia)
	while (activeSet.size() > 0) {
		std::cout << "set: ";
		for (int active : activeSet) {
			std::cout << active << " ";
		}
		std::cout << std::endl;

		// Create subset of unblocked pieces
		std::vector<int> unblocked;
		for (unsigned int j = 0; j < activeSet.size(); j++) {
			// setup booleans for the six directions
			bool xPlus = true;
			bool xMinus = true;
			bool zPlus = true;
			bool zMinus = true;
			//bool yPlus = true;
			//bool yMinus = true;
			for (Block block : nodes[activeSet[j]].blocked) {

				// if blocking part is in the active set
				if (std::find(activeSet.begin(), activeSet.end(), block.part->index) != activeSet.end()) {
					if (block.direction.x == 1.f) xPlus = false;
					else if (block.direction.x == -1.f) xMinus = false;
					else if (block.direction.z == 1.f) zPlus = false;
					else if (block.direction.z == -1.f) zMinus = false;
					//else if (block.direction.y == 1.f) yPlus = false;
					//else if (block.direction.y == -1.f)  yMinus = false;
				}
			}
			//if (((xPlus || xMinus) || (yPlus || yMinus)) || (zPlus || zMinus)) {
			if ((xPlus || xMinus) || (zPlus || zMinus)) unblocked.push_back(activeSet[j]);
		}

		// add edge to every active part that touches p
		/*for (unsigned int m = 0; m < unblocked.size(); m++) {
			activeSet.erase(std::find(activeSet.begin(), activeSet.end(), unblocked[m]));
		}*/

		//for each unblocked part
		for (unsigned int m = 0; m < unblocked.size(); m++) {
			std::vector<int> blocking;
			glm::vec3 unblockDirection;
			float minDistance = 10000.f;

			bool xPlus = true;
			bool xMinus = true;
			bool zPlus = true;
			bool zMinus = true;
			//bool yPlus = true;
			//bool yMinus = true;
			for (Block block : nodes[unblocked[m]].blocked) {
				// if blocking part is in the active set
				if (std::find(activeSet.begin(), activeSet.end(), block.part->index) != activeSet.end()) {
					blocking.push_back(block.part->index);
					if (block.direction.x == 1.f) xPlus = false;
					else if (block.direction.x == -1.f) xMinus = false;
					else if (block.direction.z == 1.f) zPlus = false;
					else if (block.direction.z == -1.f) zMinus = false;
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

			// add edge to every active part that touches p
			activeSet.erase(std::find(activeSet.begin(), activeSet.end(), unblocked[m]));


			for (Block block : nodes[unblocked[m]].blocked) {
				// if blocking part is in the active set
				if (std::find(activeSet.begin(), activeSet.end(), block.part->index) != activeSet.end()) {
					bool contains = false;
					for (Node* n : graph[block.part->index]) {
						if (n->index == unblocked[m]) contains = true;
					}
					if (!contains) {
						graph[block.part->index].push_back(&nodes[unblocked[m]]);
					}
				}
			}
			nodes[unblocked[m]].direction = unblockDirection;
			nodes[unblocked[m]].selfDistance = minDistance;
		}

	}

	constructInverse();
	if (sort() == -1) {
		std::cout << "Error, graph contains cycle(s)" << std::endl;
	}
	fillDistances();
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
void ExplosionGraph::fillDistances() {

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
			node->totalDistance = node->selfDistance + max;
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

void ExplosionGraph::hardCodedBlocking() {
	// Manually add blocking for now
	// block a
	nodes[7].blocked.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, 1.f)));//1
	nodes[7].blocked.push_back(Block(&nodes[0], glm::vec3(1.f, 0.f, 0.f)));//2
	nodes[7].blocked.push_back(Block(&nodes[0], glm::vec3(-1.f, 0.f, 0.f)));//3

	// block b
	nodes[0].blocked.push_back(Block(&nodes[7], glm::vec3(0.f, 0.f, -1.f)));//4

	nodes[0].blocked.push_back(Block(&nodes[7], glm::vec3(1.f, 0.f, 0.f)));//5
	nodes[0].blocked.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));//6
	nodes[0].blocked.push_back(Block(&nodes[2], glm::vec3(1.f, 0.f, 0.f)));//7

	nodes[0].blocked.push_back(Block(&nodes[7], glm::vec3(-1.f, 0.f, 0.f)));//8
	nodes[0].blocked.push_back(Block(&nodes[1], glm::vec3(-1.f, 0.f, 0.f)));//9
	nodes[0].blocked.push_back(Block(&nodes[2], glm::vec3(-1.f, 0.f, 0.f)));//10

	nodes[0].blocked.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, 1.f)));//11
	nodes[0].blocked.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, 1.f)));//12
	nodes[0].blocked.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, 1.f)));//13

	// block c
	nodes[1].blocked.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));//14
	nodes[1].blocked.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, -1.f)));//15
	nodes[1].blocked.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, -1.f)));//16

	nodes[1].blocked.push_back(Block(&nodes[0], glm::vec3(1.f, 0.f, 0.f)));//17
	nodes[1].blocked.push_back(Block(&nodes[6], glm::vec3(1.f, 0.f, 0.f)));//18

	nodes[1].blocked.push_back(Block(&nodes[0], glm::vec3(-1.f, 0.f, 0.f)));//19
	nodes[1].blocked.push_back(Block(&nodes[2], glm::vec3(-1.f, 0.f, 0.f)));//20
	nodes[1].blocked.push_back(Block(&nodes[3], glm::vec3(-1.f, 0.f, 0.f)));//21
	nodes[1].blocked.push_back(Block(&nodes[4], glm::vec3(-1.f, 0.f, 0.f)));//22
	nodes[1].blocked.push_back(Block(&nodes[6], glm::vec3(-1.f, 0.f, 0.f)));//23

	nodes[1].blocked.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, 1.f)));//24
	nodes[1].blocked.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, 1.f)));//25
	nodes[1].blocked.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, 1.f)));//26

	// block d
	nodes[2].blocked.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));//27
	nodes[2].blocked.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));//28

	nodes[2].blocked.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, 1.f)));//29
	nodes[2].blocked.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, 1.f)));//30

	nodes[2].blocked.push_back(Block(&nodes[0], glm::vec3(-1.f, 0.f, 0.f)));//31
	nodes[2].blocked.push_back(Block(&nodes[3], glm::vec3(-1.f, 0.f, 0.f)));//32

	nodes[2].blocked.push_back(Block(&nodes[0], glm::vec3(1.f, 0.f, 0.f)));//33
	nodes[2].blocked.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));//34

	// block e
	nodes[3].blocked.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));//35
	nodes[3].blocked.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, -1.f)));//36

	nodes[3].blocked.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, 1.f)));//37
	nodes[3].blocked.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, 1.f)));//38

	nodes[3].blocked.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));//39
	nodes[3].blocked.push_back(Block(&nodes[2], glm::vec3(1.f, 0.f, 0.f)));//40
	nodes[3].blocked.push_back(Block(&nodes[4], glm::vec3(1.f, 0.f, 0.f)));//41

	// block f
	nodes[4].blocked.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));//42
	nodes[4].blocked.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));//43
	nodes[4].blocked.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, -1.f)));//44
	nodes[4].blocked.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, -1.f)));//45

	nodes[4].blocked.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, 1.f)));//46
	nodes[4].blocked.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, 1.f)));//47

	nodes[4].blocked.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));//48
	nodes[4].blocked.push_back(Block(&nodes[6], glm::vec3(1.f, 0.f, 0.f)));//49

	nodes[4].blocked.push_back(Block(&nodes[3], glm::vec3(-1.f, 0.f, 0.f)));//50
	nodes[4].blocked.push_back(Block(&nodes[6], glm::vec3(-1.f, 0.f, 0.f)));//51

	// block g
	nodes[6].blocked.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));//52
	nodes[6].blocked.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, -1.f)));//53
	nodes[6].blocked.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, -1.f)));//54

	nodes[6].blocked.push_back(Block(&nodes[5], glm::vec3(0.f, 0.f, 1.f)));//55

	nodes[6].blocked.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));//56
	nodes[6].blocked.push_back(Block(&nodes[4], glm::vec3(1.f, 0.f, 0.f)));//57
	nodes[6].blocked.push_back(Block(&nodes[5], glm::vec3(1.f, 0.f, 0.f)));//58

	nodes[6].blocked.push_back(Block(&nodes[1], glm::vec3(-1.f, 0.f, 0.f)));//59
	nodes[6].blocked.push_back(Block(&nodes[4], glm::vec3(-1.f, 0.f, 0.f)));//60
	nodes[6].blocked.push_back(Block(&nodes[5], glm::vec3(-1.f, 0.f, 0.f)));//61

	// block h
	nodes[5].blocked.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, -1.f)));//62
	nodes[5].blocked.push_back(Block(&nodes[6], glm::vec3(1.f, 0.f, 0.f)));//63
	nodes[5].blocked.push_back(Block(&nodes[6], glm::vec3(-1.f, 0.f, 0.f)));//64
}
