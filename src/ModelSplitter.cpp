#include "ModelSplitter.h"

std::vector<Renderable*> ModelSplitter::split(Renderable* mainObject) {

	// Structures for tracking faces and verts that have been processed
	std::vector<bool> vertsProcessed(mainObject->verts.size(), false);
	std::vector<bool> facesProcessed(mainObject->vertIndices.size(), false);
	unsigned int numVertsProcessed = 0;

	// Split objects until all verts have been processed
	std::vector<Renderable*> splitObjects;
	while (numVertsProcessed < mainObject->verts.size()) {

		// Create a new renderable
		Renderable* r = new Renderable();
		splitObjects.push_back(r);

		// Create queue of verts to process and push a unprocessed vert to start with
		std::deque<unsigned short> vertsToProcess;
		std::vector<bool> facesUpdated;
		for (unsigned int i = 0; i < vertsProcessed.size(); i++) {
			if (!vertsProcessed[i]) {
				vertsToProcess.push_back(i);
				break;
			}
		}
		// Process all verts in the queue
		while (vertsToProcess.size() != 0) {
			for (unsigned int j = 0; j < mainObject->vertIndices.size(); j++) {
				if (mainObject->vertIndices[j] == vertsToProcess.front()) {
					unsigned int index = j % 3;
					unsigned int first;
					unsigned int second;
					unsigned int third;
					unsigned int secondIndex;
					unsigned int thirdIndex;
					if (index == 0) {
						first = j;
						second = j + 1;
						third = j + 2;
						secondIndex = j + 1;
						thirdIndex = j + 2;
					} else if (index == 1) {
						first = j - 1;
						second = j;
						third = j + 1;
						secondIndex = j - 1;
						thirdIndex = j + 1;
					} else {
						first = j - 2;
						second = j - 1;
						third = j;
						secondIndex = j - 2;
						thirdIndex = j - 1;
					}
					bool inListSecond = false;
					bool inListThird = false;
					//Change this so that we check vertsProcessed first
					for (unsigned int k = 0; k < vertsToProcess.size(); k++) {
						if (mainObject->vertIndices[secondIndex] == vertsToProcess[k] || vertsProcessed[mainObject->vertIndices[secondIndex]]) {
							inListSecond = true;
						}
						if (mainObject->vertIndices[thirdIndex] == vertsToProcess[k] || vertsProcessed[mainObject->vertIndices[thirdIndex]]) {
							inListThird = true;
						}
						if (inListSecond && inListThird) {
							break;
						}
					}
					if (!inListSecond) {
						vertsToProcess.push_back(mainObject->vertIndices[secondIndex]);
					}
					if (!inListThird) {
						vertsToProcess.push_back(mainObject->vertIndices[thirdIndex]);
					}
					if (!facesProcessed[first]) {
						facesProcessed[first] = true;
						r->vertIndices.push_back(mainObject->vertIndices[first]);
						facesUpdated.push_back(false);
						r->vertIndices.push_back(mainObject->vertIndices[second]);
						facesUpdated.push_back(false);
						r->vertIndices.push_back(mainObject->vertIndices[third]);
						facesUpdated.push_back(false);

						r->normalIndices.push_back(mainObject->normalIndices[first]);
						r->normalIndices.push_back(mainObject->normalIndices[second]);
						r->normalIndices.push_back(mainObject->normalIndices[third]);
						r->uvIndices.push_back(mainObject->uvIndices[first]);
						r->uvIndices.push_back(mainObject->uvIndices[second]);
						r->uvIndices.push_back(mainObject->uvIndices[third]);
					}
				}
			}
			vertsProcessed[vertsToProcess.front()] = true;
			numVertsProcessed++;
			vertsToProcess.pop_front();
		}
		// Push back data for new object
		for (unsigned int i = 0; i < r->vertIndices.size(); i++) {
			r->verts.push_back(mainObject->verts[r->vertIndices[i]]);
			r->normals.push_back(mainObject->normals[r->normalIndices[i]]);
			r->uvs.push_back(mainObject->uvs[r->uvIndices[i]]);
		}
	}
	return splitObjects;
}
