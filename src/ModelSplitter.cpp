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
	for (unsigned int i = 0; i < object->rawVerts.size(); i++) {
		vertsProcessed.push_back(false);
	}
	unsigned int numVertsProcessed = 0;

	std::vector<bool> facesProcessed;
	for (unsigned int i = 0; i < object->faces.size(); i++) {
		facesProcessed.push_back(false);
	}

	std::vector<Renderable*> splitObjects;
	while (numVertsProcessed < object->rawVerts.size()) {
		Renderable* newObject = new Renderable();
		splitObjects.push_back(newObject);
		std::vector<glm::vec3> rawVerts;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<GLushort> faces;
		std::vector<GLushort> normalIndices;
		std::vector<GLushort> uvIndices;
		std::vector<glm::vec3> drawVerts;
		std::vector<unsigned int> vertsToProcess;
		std::vector<bool> facesUpdated;
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
					//Change this so that we check vertsProcessed first
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
						facesUpdated.push_back(false);
						faces.push_back(object->faces[second]);
						facesUpdated.push_back(false);
						faces.push_back(object->faces[third]);
						facesUpdated.push_back(false);
						normalIndices.push_back(object->normalIndices[first]);
						normalIndices.push_back(object->normalIndices[second]);
						normalIndices.push_back(object->normalIndices[third]);
						uvIndices.push_back(object->uvIndices[first]);
						uvIndices.push_back(object->uvIndices[second]);
						uvIndices.push_back(object->uvIndices[third]);
						//Push normal and uv indices here too
					}
				}
			}
			rawVerts.push_back(object->rawVerts[vertsToProcess[0]]);
			//Repeat the below loop for normals and uvs
			for (unsigned int i = 0; i < faces.size(); i++) {
				if (faces[i] == vertsToProcess[0] && facesUpdated[i] == false) {
					faces[i] = rawVerts.size() - 1;
					facesUpdated[i] = true;
				}
			}
			vertsProcessed[vertsToProcess[0]] = true;
			numVertsProcessed++;
			vertsToProcess.erase(vertsToProcess.begin());
		}
		for (unsigned int i = 0; i < rawVerts.size(); i++) {
			newObject->rawVerts.push_back(rawVerts[i]);
		}
		for (unsigned int i = 0; i < faces.size(); i++) {
			newObject->drawVerts.push_back(rawVerts[faces[i]]);
		}
		for (unsigned int i = 0; i < faces.size(); i++) {
			newObject->normals.push_back(object->normals[normalIndices[i]]);
		}
		for (unsigned int i = 0; i < faces.size(); i++) {
			newObject->uvs.push_back(object->uvs[uvIndices[i]]);
		}
		for (unsigned int i = 0; i < normals.size(); i++) {
			newObject->normalIndices.push_back(normalIndices[i]);
		}
		for (unsigned int i = 0; i < uvs.size(); i++) {
			newObject->uvIndices.push_back(uvIndices[i]);
		}
		for (unsigned int i = 0; i < faces.size(); i++) {
			newObject->faces.push_back(faces[i]);
		}
	}
	return splitObjects;
}
