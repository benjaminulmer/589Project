#include "ExplosionGraph.h"

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
	sort();
	fillDistacnes();
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
	}
	// **** to add an edge: graph[parentIndex].push_back(&nodes[childIndex]) ***** //

	// Compute contacts and blocking (John)

	// Construct graph from contacts and blocking (Mia)

	constructInverse();

	// Compute total distance for each part after graph is complete (Ben)
	fillDistacnes();
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

#include <iostream>

// Topologically sorts the graph
void ExplosionGraph::sort() {
	std::vector<std::list<Node*>> iGraphMain = iGraph;

	unsigned int numAdded = 0;
	std::vector<Node*> queue;

	while (numAdded < nodes.size()) {
		std::vector<std::list<Node*>> iGraphC = iGraphMain;

		for (unsigned int i = 0; i < iGraphC.size(); i++) {
			std::list<Node*> children = graph[i];
			std::list<Node*> parents = iGraphC[i];

			// One parent (only self) means no incoming edges
			if (parents.size() == 1) {
				queue.push_back(parents.front());
				numAdded++;

				for (Node* child : children) {
					iGraphMain[child->index].remove(parents.front());
				}
			}
		}
		topologicalSort.push_back(queue);
		queue.clear();
	}
	for (std::vector<Node*> level : topologicalSort) {
		for (Node* n : level) {
			std::cout << n->index << ", ";
		}
		std::cout << std::endl;
	}
}

// Fills in total distance of each node in graph based off of parent distances
void ExplosionGraph::fillDistacnes() {

}

// Returns topologically sorted graph. Nodes of same depth are grouped together inside inner vector
std::vector<std::vector<Node*>>& ExplosionGraph::getSort() {
	return topologicalSort;
}