#include "ExplosionGraph.h"

#include <iostream>

// Default constructor. Zeros everything
Node::Node() : part(0), index(0), selfDistance(0.0f), totalDistance(0.0f) {}

// Node for part with given index
Node::Node(Renderable* part, int index) : part(part), index(index), selfDistance(0.0f), totalDistance(0.0f) {}

// Creates hard coded explosion graph for testing
ExplosionGraph::ExplosionGraph() {

	// Number of nodes in graph is number of parts in model
	int numParts = 5;
	graph = std::vector<std::list<Node*>>(numParts);
	iGraph = std::vector<std::list<Node*>>(numParts);

	// Fill list of nodes
	nodes = std::vector<Node>(numParts);
	for (int i = 0; i < numParts; i++) {
		nodes[i] = Node(nullptr, i);
		graph[i].push_back(&nodes[i]);
	}

	// Hard coded graph for testing

	/*
	graph[1].push_back(&nodes[0]); graph[1].push_back(&nodes[4]);
	graph[2].push_back(&nodes[1]); graph[2].push_back(&nodes[3]); graph[2].push_back(&nodes[5]); graph[2].push_back(&nodes[6]); graph[2].push_back(&nodes[4]);
	graph[3].push_back(&nodes[1]); graph[3].push_back(&nodes[4]);
	graph[5].push_back(&nodes[4]); graph[5].push_back(&nodes[6]);
	graph[6].push_back(&nodes[7]); graph[6].push_back(&nodes[4]);
	*/

	graph[1].push_back(&nodes[0]); graph[1].push_back(&nodes[2]);
	graph[2].push_back(&nodes[0]);
	graph[3].push_back(&nodes[1]); graph[3].push_back(&nodes[2]);
	graph[4].push_back(&nodes[1]); graph[4].push_back(&nodes[3]);


	for (unsigned int i = 0; i < nodes.size(); i++) {
		nodes[i].direction = glm::vec3(0.0f, -1.0f, 0.0f);
	}
	nodes[0].selfDistance = 0.2f;
	nodes[1].selfDistance = 2.0f;
	nodes[2].selfDistance = 1.0f;
	nodes[3].selfDistance = 1.0f;
	nodes[4].selfDistance = 0.0f;

	constructInverse();
	if (sort() == -1) {
		std::cout << "Error, graph contains cycle(s)" << std::endl;
	}
	fillDistances();
}

// Creates explosion graph for provided parts
ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts) {

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
	// **** to add an edge: graph[parentIndex].push_back(&nodes[childIndex]) ***** //

	// Compute contacts and blocking (John)

	// Construct graph from contacts and blocking (Mia)

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
