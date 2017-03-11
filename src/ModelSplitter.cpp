/*
 * ModelSplitter.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: jshall
 */

#include "ModelSplitter.h"

#include "Renderable.h"

ModelSplitter::ModelSplitter() {
	// TODO Auto-generated constructor stub

}

ModelSplitter::~ModelSplitter() {
	// TODO Auto-generated destructor stub
}

std::vector<Renderable*> ModelSplitter::split(Renderable* object) {
	std::vector<bool> vertsProcessed;
	for (unsigned int i = 0; i < object->verts.size(); i++) {
		vertsProcessed.push_back(false);
	}

	std::vector<GLushort> facesProcessed;
	for (unsigned int i = 0; i < object->faces.size(); i++) {
		facesProcessed.push_back(false);
	}

	std::vector<Renderable*> splitObjects;
	while (!andVector(vertsProcessed)) {
		Renderable* newObject = new Renderable();
		splitObjects.push_back(newObject);
		std::vector<glm::vec3> verts;
		std::vector<GLushort> faces;
		std::vector<unsigned int> vertsToProcess;
		for (unsigned int i = 0; i < vertsProcessed.size(); i++) {
			if (!vertsProcessed[i]) {
				vertsToProcess.push_back(i);
				break;
			}
		}
		while (vertsToProcess.size() != 0) {
			for (unsigned int j = 0; j < object->faces.size(); j++) {
				if (object->faces[j] == vertsToProcess[0]) {
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
					for (unsigned int k = 0; k < vertsToProcess.size(); k++) {
						if (object->faces[secondIndex] == vertsToProcess[k] || vertsProcessed[object->faces[secondIndex]]) {
							inListSecond = true;
						}
						if (object->faces[thirdIndex] == vertsToProcess[k] || vertsProcessed[object->faces[thirdIndex]]) {
							inListThird = true;
						}
						if (inListSecond && inListThird) {
							break;
						}
					}
					if (!inListSecond) {
						vertsToProcess.push_back(object->faces[secondIndex]);
					}
					if (!inListThird) {
						vertsToProcess.push_back(object->faces[thirdIndex]);
					}
					if (!facesProcessed[first]) {
						facesProcessed[first] = true;
						faces.push_back(object->faces[first]);
						faces.push_back(object->faces[second]);
						faces.push_back(object->faces[third]);
					}
				}
			}
			verts.push_back(object->verts[vertsToProcess[0]]);
			for (unsigned int i = 0; i < faces.size(); i++) {
				if (faces[i] == vertsToProcess[0]) {
					faces[i] = verts.size() - 1;
				}
			}
			vertsProcessed[vertsToProcess[0]] = true;
			vertsToProcess.erase(vertsToProcess.begin());
		}
		for (unsigned int i = 0; i < verts.size(); i++) {
			newObject->verts.push_back(verts[i]);
		}
		for (unsigned int i = 0; i < faces.size(); i++) {
			newObject->faces.push_back(faces[i]);
		}
	}
	return splitObjects;
}

bool ModelSplitter::andVector(std::vector<bool> input) {
	bool answer = true;
	for (unsigned int i = 0; i < input.size(); i++) {
		answer = answer && input[i];
	}
	return answer;
}

