#include "ExplosionGraph.h"

// Creates hard coded explosion graph for testing
ExplosionGraph::ExplosionGraph() : isSorted(false) {

}

// Creates explosion graph for provided parts
ExplosionGraph::ExplosionGraph(std::vector<Renderable*> parts) : isSorted(false) {
	// Compute contacts and blocking (John)

	// Construct graph from contacts and blocking (Mia)

	// Compute total distance for each part after graph is complete (Ben)
	fillDistacnes();

	// Construct inverse graph from completed graph - trivial (anyone)
}

// Fills in total distance of each node in graph based off of parent distances
void ExplosionGraph::fillDistacnes() {
	// Code goes here
}

// Returns topologically sorted graph. Nodes of same depth are grouped together inside inner vector
std::vector<std::vector<Node>>& ExplosionGraph::getSort() {
	if (!isSorted) {
		// Code goes here (Ben)

		isSorted = true;
	}
	return topologicalSort;
}
