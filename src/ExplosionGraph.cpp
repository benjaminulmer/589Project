#include "ExplosionGraph.h"

// Creates hard coded explosion graph for testing
ExplosionGraph::ExplosionGraph() : isSorted(false) {

	// Number of nodes in graph is number of parts in model
	int numParts = 8;
	graph = std::vector<std::list<Node>>(numParts);
	iGraph = std::vector<std::list<Node>>(numParts);

	// Fill list of nodes
	nodes = std::vector<Node>(numParts);
	for (int i = 0; i < numParts; i++) {
		Node node;
		node.part = 0;
		node.index = i;
		nodes[i] = node;
	}

	// Hard coded graph for testing
	graph[1].push_back(&nodes[0]); graph[1].push_back(&nodes[4]);
	graph[2].push_back(&nodes[1]); graph[2].push_back(&nodes[3]); graph[2].push_back(&nodes[5]); graph[2].push_back(&nodes[6]); graph[2].push_back(&nodes[4]);
	graph[3].push_back(&nodes[1]); graph[3].push_back(&nodes[4]);
	graph[5].push_back(&nodes[4]); graph[5].push_back(&nodes[6]);
	graph[6].push_back(&nodes[7]); graph[6].push_back(&nodes[4]);

	constructInverse();
	fillDistacnes();
}

// Creates explosion graph for provided parts
ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts) : isSorted(false) {

	// Number of nodes in graph is number of parts in model
	int numParts = parts.size();
	graph = std::vector<std::list<Node>>(numParts);
	iGraph = std::vector<std::list<Node>>(numParts);

	// Fill list of nodes
	nodes = std::vector<Node>(numParts);
	for (int i = 0; i < numParts; i++) {
		Node node;
		node.part = parts[i];
		node.index = i;
		nodes[i] = node;
	}
	// **** to add an edge: graph[parentIndex].push_back(&nodes[childIndex]) ***** //

	// Compute contacts and blocking (John)

	// Construct graph from contacts and blocking (Mia)

	constructInverse();

	// Compute total distance for each part after graph is complete (Ben)
	fillDistacnes();
}

// Fills in total distance of each node in graph based off of parent distances
void ExplosionGraph::fillDistacnes() {
	int i = 0;
	for (std::list<Node> node : graph) {
		//std::list<Node> parents = iGraph

		i++;
	}
}

// Constructs inverse graph from normal graph
void ExplosionGraph::constructInverse() {
	int i = 0;
	for (std::list<Node> node : graph) {
		for (Node neighbour : node) {
			iGraph[neighbour.index].push_back(&nodes[i]);
		}
		i++;
	}
}

// Returns topologically sorted graph. Nodes of same depth are grouped together inside inner vector
std::vector<std::vector<Node*>>& ExplosionGraph::getSort() {
	if (!isSorted) {
		// Code goes here (Ben)

		isSorted = true;
	}
	return topologicalSort;
}
