#include "ExplosionGraph.h"

#include <iostream>

// Default constructor. Zeros everything
Node::Node() : part(0), index(0), selfDistance(0.0f), totalDistance(0.0f) {}

// Node for part with given index
Node::Node(Renderable* part, int index) : part(part), index(index), selfDistance(0.0f), totalDistance(0.0f) {}

ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts, bool test) {

	// Number of nodes in graph is number of parts in model
	int numParts = parts.size();
	graph = std::vector<std::list<Node*>>(numParts);
	iGraph = std::vector<std::list<Node*>>(numParts);

	// Fill list of nodes
	nodes = std::vector<Node>(numParts);
	for (int i = 0; i < numParts; i++) {
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

// Creates hard coded explosion graph for testing
ExplosionGraph::ExplosionGraph() { }

// Creates explosion graph for provided parts
ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts) {

	// Number of nodes in graph is number of parts in model
	int numParts = parts.size();
	graph = std::vector<std::list<Node*>>(numParts);
	iGraph = std::vector<std::list<Node*>>(numParts);

	// Fill list of nodes
	nodes = std::vector<Node>(numParts);
	std::vector<int> activeSet;
	for (int i = 0; i < numParts; i++) {
		nodes[i] = Node(parts[i], i);
		graph[i].push_back(&nodes[i]);
		activeSet.push_back(i);
	}
	// **** to add an edge: graph[parentIndex].push_back(&nodes[childIndex]) ***** //

	// Compute contacts and blocking (John)

	// Manually add blocking for now
	// block a
	nodes[0].blocking.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, 1.f)));
	nodes[0].blocking.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));
	nodes[0].blocking.push_back(Block(&nodes[1], glm::vec3(-1.f, 0.f, 0.f)));

	// block b
	nodes[1].blocking.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));
	
	nodes[1].blocking.push_back(Block(&nodes[0], glm::vec3(1.f, 0.f, 0.f)));
	nodes[1].blocking.push_back(Block(&nodes[2], glm::vec3(1.f, 0.f, 0.f)));
	nodes[1].blocking.push_back(Block(&nodes[3], glm::vec3(1.f, 0.f, 0.f)));

	nodes[1].blocking.push_back(Block(&nodes[0], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[1].blocking.push_back(Block(&nodes[2], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[1].blocking.push_back(Block(&nodes[3], glm::vec3(-1.f, 0.f, 0.f)));

	nodes[1].blocking.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, 1.f)));
	nodes[1].blocking.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, 1.f)));
	nodes[1].blocking.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, 1.f)));

	// block c
	nodes[2].blocking.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));
	nodes[2].blocking.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, -1.f)));
	nodes[2].blocking.push_back(Block(&nodes[5], glm::vec3(0.f, 0.f, -1.f)));

	nodes[2].blocking.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));
	nodes[2].blocking.push_back(Block(&nodes[6], glm::vec3(1.f, 0.f, 0.f)));

	nodes[2].blocking.push_back(Block(&nodes[1], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[2].blocking.push_back(Block(&nodes[3], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[2].blocking.push_back(Block(&nodes[4], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[2].blocking.push_back(Block(&nodes[5], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[2].blocking.push_back(Block(&nodes[6], glm::vec3(-1.f, 0.f, 0.f)));

	nodes[2].blocking.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, 1.f)));
	nodes[2].blocking.push_back(Block(&nodes[5], glm::vec3(0.f, 0.f, 1.f)));
	nodes[2].blocking.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, 1.f)));

	// block d
	nodes[3].blocking.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));
	nodes[3].blocking.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));
	nodes[3].blocking.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, -1.f)));

	nodes[3].blocking.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, 1.f)));
	nodes[3].blocking.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, 1.f)));
	nodes[3].blocking.push_back(Block(&nodes[5], glm::vec3(0.f, 0.f, 1.f)));
	nodes[3].blocking.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, 1.f)));
	nodes[3].blocking.push_back(Block(&nodes[7], glm::vec3(0.f, 0.f, 1.f)));

	nodes[3].blocking.push_back(Block(&nodes[1], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[3].blocking.push_back(Block(&nodes[4], glm::vec3(-1.f, 0.f, 0.f)));

	nodes[3].blocking.push_back(Block(&nodes[1], glm::vec3(1.f, 0.f, 0.f)));
	nodes[3].blocking.push_back(Block(&nodes[2], glm::vec3(1.f, 0.f, 0.f)));

	// block e
	nodes[4].blocking.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));
	nodes[4].blocking.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, -1.f)));

	nodes[4].blocking.push_back(Block(&nodes[5], glm::vec3(0.f, 0.f, 1.f)));
	nodes[4].blocking.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, 1.f)));

	nodes[4].blocking.push_back(Block(&nodes[2], glm::vec3(1.f, 0.f, 0.f)));
	nodes[4].blocking.push_back(Block(&nodes[3], glm::vec3(1.f, 0.f, 0.f)));
	nodes[4].blocking.push_back(Block(&nodes[5], glm::vec3(1.f, 0.f, 0.f)));

	// block f
	nodes[5].blocking.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));
	nodes[5].blocking.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));
	nodes[5].blocking.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, -1.f)));
	nodes[5].blocking.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, -1.f)));
	nodes[5].blocking.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, -1.f)));

	nodes[5].blocking.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, 1.f)));
	nodes[5].blocking.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, 1.f)));
	nodes[5].blocking.push_back(Block(&nodes[7], glm::vec3(0.f, 0.f, 1.f)));

	nodes[5].blocking.push_back(Block(&nodes[2], glm::vec3(1.f, 0.f, 0.f)));
	nodes[5].blocking.push_back(Block(&nodes[6], glm::vec3(1.f, 0.f, 0.f)));

	nodes[5].blocking.push_back(Block(&nodes[4], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[5].blocking.push_back(Block(&nodes[6], glm::vec3(-1.f, 0.f, 0.f)));

	// block g
	nodes[6].blocking.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));
	nodes[6].blocking.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));
	nodes[6].blocking.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, -1.f)));
	nodes[6].blocking.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, -1.f)));
	nodes[6].blocking.push_back(Block(&nodes[4], glm::vec3(0.f, 0.f, -1.f)));
	nodes[6].blocking.push_back(Block(&nodes[5], glm::vec3(0.f, 0.f, -1.f)));

	nodes[6].blocking.push_back(Block(&nodes[7], glm::vec3(0.f, 0.f, 1.f)));

	nodes[6].blocking.push_back(Block(&nodes[2], glm::vec3(1.f, 0.f, 0.f)));
	nodes[6].blocking.push_back(Block(&nodes[5], glm::vec3(1.f, 0.f, 0.f)));
	nodes[6].blocking.push_back(Block(&nodes[7], glm::vec3(1.f, 0.f, 0.f)));

	nodes[6].blocking.push_back(Block(&nodes[2], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[6].blocking.push_back(Block(&nodes[5], glm::vec3(-1.f, 0.f, 0.f)));
	nodes[6].blocking.push_back(Block(&nodes[7], glm::vec3(-1.f, 0.f, 0.f)));

	// block h
	nodes[7].blocking.push_back(Block(&nodes[0], glm::vec3(0.f, 0.f, -1.f)));
	nodes[7].blocking.push_back(Block(&nodes[1], glm::vec3(0.f, 0.f, -1.f)));
	nodes[7].blocking.push_back(Block(&nodes[2], glm::vec3(0.f, 0.f, -1.f)));
	nodes[7].blocking.push_back(Block(&nodes[3], glm::vec3(0.f, 0.f, -1.f)));
	nodes[7].blocking.push_back(Block(&nodes[5], glm::vec3(0.f, 0.f, -1.f)));
	nodes[7].blocking.push_back(Block(&nodes[6], glm::vec3(0.f, 0.f, -1.f)));

	nodes[7].blocking.push_back(Block(&nodes[6], glm::vec3(1.f, 0.f, 0.f)));
	
	nodes[7].blocking.push_back(Block(&nodes[6], glm::vec3(-1.f, 0.f, 0.f)));

	// Construct graph from contacts and blocking (Mia)
	int i = 0;
	while (activeSet.size() > 0) {
		std::cout << "Iteration " << i << std::endl;
		for (int active : activeSet) {
			std::cout << active << " ";
		}
		std::cout << std::endl;

		// Create subset of unblocked pieces
		std::vector<int> unblocked;
		for (int j = 0; j < activeSet.size(); j++) {
			// setup booleans for the six directions
			bool xPlus = true;
			bool xMinus = true;
			bool zPlus = true;
			bool zMinus = true;
			bool yPlus = false; // for now
			bool yMinus = false; // for now
			for (Block block : nodes[activeSet[j]].blocking) {

				// if blocking part is in the active set
				if (std::find(activeSet.begin(), activeSet.end(), block.part->index) != activeSet.end()) {
					//TODO Make this better
					if (block.direction.x == 1.f) {
						//std::cout << "blocked in x+ direction" << std::endl;

						xPlus = false;
						nodes[activeSet[j]].direction = glm::vec3(1.f, 0.f, 0.f);
					}
					else if (block.direction.x == -1.f) {
						//std::cout << "blocked in x- direction" << std::endl;

						xMinus = false;
						nodes[activeSet[j]].direction = glm::vec3(-1.f, 0.f, 0.f);
					}
					else if (block.direction.z == 1.f) {
						//std::cout << "blocked in z+ direction" << std::endl;

						zPlus = false;
						nodes[activeSet[j]].direction = glm::vec3(0.f, 0.f, 1.f);
					}
					else if (block.direction.z == -1.f) {
						//std::cout << "blocked in z- direction" << std::endl;

						zMinus = false;
						nodes[activeSet[j]].direction = glm::vec3(0.f, 0.f, -1.f);
					}
					/*else if (block.direction.y == 1.f){
						yPlus = false;
						nodes[activeSet[j]].direction = glm::vec3(0.f, 1.f, 0.f);
					}
					else if (block.direction.y == -1.f) {
						yMinus = false;
						nodes[activeSet[j]].direction = glm::vec3(0.f, -1.f, 0.f);
					}*/
				}
			}
			if (((xPlus || xMinus) || (yPlus || yMinus)) || (zPlus || zMinus)) {
				std::cout << "Adding " << activeSet[j] << " to remove nodes" << std::endl;

				unblocked.push_back(activeSet[j]);
			}
		}
		// now the set of unblocked parts is created

		//for each unblocked part
		for (int m = 0; m < unblocked.size(); m++) {
			//TODO determine shortest distance needed to escape bounding box of contacting parts,
			// & store the direction of this distance as the explosion direction for p
			activeSet.erase(std::find(activeSet.begin(), activeSet.end(), unblocked[m]));
			// add edge to every active part that touches p
		}
		i++;
	}
	
	constructInverse();

	// Compute total distance for each part after graph is complete (Ben)
	if (sort() == -1) {
		std::cout << "Error, graph contains cycle(s)" << std::endl;
	}
	fillDistances();
}

// Constructs inverse graph from normal graph
void ExplosionGraph::constructInverse() {

	// Add self for each edge list
	for (unsigned int i = 0; i < nodes.size(); i++) {
		iGraph[i].push_back(&nodes[i]);
	}

	// Construct inverse
	for (unsigned int i = 0; i < nodes.size(); i++) {
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
	while (numAdded < nodes.size()) {
		bool found = false;

		// Another copy so all nodes of can be found for each level
		std::vector<std::list<Node*>> iGraphC = iGraphMain;

		// Loop over all nodes in graph
		for (unsigned int i = 0; i < iGraphC.size(); i++) {
			Node* self = iGraphC[i].front();

			// One parent (only self) means no incoming edges. 0 means it has been removed
			if (iGraphC[i].size() == 1) {
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
