#include "ModelOperations.h"

#include <stdio.h>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/intersect.hpp>

// Constructor for a block, always needs part and direction
ContactPair::ContactPair(unsigned int focusPart, unsigned int otherPart, glm::vec3 direction) : focusPart(focusPart), otherPart(otherPart), direction(direction) {}


BlockingPair::BlockingPair(unsigned int focusPart, unsigned int otherPart, glm::vec3 direction) : focusPart(focusPart), otherPart(otherPart), direction(direction) {}

std::vector<UnpackedLists> ModelOperations::split(IndexedLists& mainObject) {

	// Structures for tracking faces and verts that have been processed
	std::vector<bool> vertsProcessed(mainObject.verts.size(), false);
	std::vector<bool> facesProcessed(mainObject.vertIndices.size(), false);
	unsigned int numVertsProcessed = 0;

	// Split objects until all verts have been processed
	std::vector<UnpackedLists> splitObjects;
	while (numVertsProcessed < mainObject.verts.size()) {

		// Create a new renderable
		UnpackedLists r;
		std::vector<GLushort> vIndices;
		std::vector<GLushort> nIndices;
		std::vector<GLushort> uvIndices;

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
			for (unsigned int j = 0; j < mainObject.vertIndices.size(); j++) {
				if (mainObject.vertIndices[j] == vertsToProcess.front()) {
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
						if (mainObject.vertIndices[secondIndex] == vertsToProcess[k] || vertsProcessed[mainObject.vertIndices[secondIndex]]) {
							inListSecond = true;
						}
						if (mainObject.vertIndices[thirdIndex] == vertsToProcess[k] || vertsProcessed[mainObject.vertIndices[thirdIndex]]) {
							inListThird = true;
						}
						if (inListSecond && inListThird) {
							break;
						}
					}
					if (!inListSecond) {
						vertsToProcess.push_back(mainObject.vertIndices[secondIndex]);
					}
					if (!inListThird) {
						vertsToProcess.push_back(mainObject.vertIndices[thirdIndex]);
					}
					if (!facesProcessed[first]) {
						facesProcessed[first] = true;
						vIndices.push_back(mainObject.vertIndices[first]);
						facesUpdated.push_back(false);
						vIndices.push_back(mainObject.vertIndices[second]);
						facesUpdated.push_back(false);
						vIndices.push_back(mainObject.vertIndices[third]);
						facesUpdated.push_back(false);

						nIndices.push_back(mainObject.normalIndices[first]);
						nIndices.push_back(mainObject.normalIndices[second]);
						nIndices.push_back(mainObject.normalIndices[third]);
						uvIndices.push_back(mainObject.uvIndices[first]);
						uvIndices.push_back(mainObject.uvIndices[second]);
						uvIndices.push_back(mainObject.uvIndices[third]);
					}
				}
			}
			vertsProcessed[vertsToProcess.front()] = true;
			numVertsProcessed++;
			vertsToProcess.pop_front();
		}
		// Push back data for new object
		for (unsigned int i = 0; i < vIndices.size(); i++) {
			r.verts.push_back(mainObject.verts[vIndices[i]]);
			r.normals.push_back(mainObject.normals[nIndices[i]]);
			r.uvs.push_back(mainObject.uvs[uvIndices[i]]);
		}

		splitObjects.push_back(r);
	}

	return splitObjects;
}

std::vector<ContactPair> ModelOperations::contacts(std::vector<UnpackedLists>& objects) {
	std::vector<ContactPair> contacts;

	for (unsigned int focusObject = 0; focusObject < objects.size(); focusObject++) {
		for (unsigned int otherObject = focusObject + 1; otherObject < objects.size(); otherObject++) {
			for (unsigned int focusTriangle = 0; focusTriangle < objects[focusObject].verts.size(); focusTriangle+=3) {
				for (unsigned int otherTriangle = 0; otherTriangle < objects[otherObject].verts.size(); otherTriangle+=3) {
					glm::vec3 focusTriangleVerts[3];
					focusTriangleVerts[0] = objects[focusObject].verts[focusTriangle];
					focusTriangleVerts[1] = objects[focusObject].verts[focusTriangle + 1];
					focusTriangleVerts[2] = objects[focusObject].verts[focusTriangle + 2];

					glm::vec3 otherTriangleVerts[3];
					otherTriangleVerts[0] = objects[otherObject].verts[otherTriangle];
					otherTriangleVerts[1] = objects[otherObject].verts[otherTriangle + 1];
					otherTriangleVerts[2] = objects[otherObject].verts[otherTriangle + 2];

					//TODO Compute blocking information here

					//Compute face normals of the two triangles
					//Follow right hand rule
					glm::vec3 focusTriangleNormal = glm::normalize(glm::cross(focusTriangleVerts[0] - focusTriangleVerts[1], focusTriangleVerts[2] - focusTriangleVerts[1]));

					glm::vec3 otherTriangleNormal = glm::normalize(glm::cross(otherTriangleVerts[0] - otherTriangleVerts[1], otherTriangleVerts[2] - otherTriangleVerts[1]));

					//Decide if the normalized normals are the same
					//if (focusTriangleNormal.x == -otherTriangleNormal.x && focusTriangleNormal.y == -otherTriangleNormal.y && focusTriangleNormal.z == -otherTriangleNormal.z) {
						glm::vec3 offset = focusTriangleVerts[0] - otherTriangleVerts[0];
						//Test if the triangles are in the same plane
						//if (glm::dot(focusTriangleNormal, offset) == 0.0) {
							//Determine if the triangles intersect
							bool intersect = false;
							int numIntersect = 0;
							if (lineIntersect3D(focusTriangleVerts[0],focusTriangleVerts[1],otherTriangleVerts[0],otherTriangleVerts[1])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[0],focusTriangleVerts[1],otherTriangleVerts[0],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[0],focusTriangleVerts[1],otherTriangleVerts[1],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[0],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[1])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[0],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[0],focusTriangleVerts[2],otherTriangleVerts[1],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[1],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[1])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[1],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
							}
							if (lineIntersect3D(focusTriangleVerts[1],focusTriangleVerts[2],otherTriangleVerts[1],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
							}
							//Triangles that only share one edge may not be in contact
							if (numIntersect > 1) {
								intersect = true;
							}
							bool inTri = true;
							inTri = inTri && pointInTriangle3D(focusTriangleVerts[0],focusTriangleVerts[1],focusTriangleVerts[2], otherTriangleVerts[0]);
							inTri = inTri && pointInTriangle3D(focusTriangleVerts[0],focusTriangleVerts[1],focusTriangleVerts[2], otherTriangleVerts[1]);
							inTri = inTri && pointInTriangle3D(focusTriangleVerts[0],focusTriangleVerts[1],focusTriangleVerts[2], otherTriangleVerts[2]);
							if (inTri) {
								intersect = true;
							}
							inTri = true;
							inTri = inTri && pointInTriangle3D(otherTriangleVerts[0],otherTriangleVerts[1],otherTriangleVerts[2], focusTriangleVerts[0]);
							inTri = inTri && pointInTriangle3D(otherTriangleVerts[0],otherTriangleVerts[1],otherTriangleVerts[2], focusTriangleVerts[1]);
							inTri = inTri && pointInTriangle3D(otherTriangleVerts[0],otherTriangleVerts[1],otherTriangleVerts[2], focusTriangleVerts[2]);
							if (inTri) {
								intersect = true;
							}
							if (intersect) {
								//Determine blocking direction(s)
								float angle = glm::dot(focusTriangleNormal, glm::vec3(1, 0, 0));
								if (angle > 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an acute angle, focus object cannot move in positive x, other cannot move in -x
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i].focusPart == focusObject && contacts[i].otherPart == otherObject && contacts[i].direction == glm::vec3(-1, 0, 0)) {
											alreadyExists = true;
										}
										if (contacts[i].focusPart == otherObject && contacts[i].otherPart == focusObject && contacts[i].direction == glm::vec3(1, 0, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										contacts.push_back(ContactPair(focusObject, otherObject, glm::vec3(-1, 0, 0)));
									}
									if (!alreadyExists2) {
										contacts.push_back(ContactPair(otherObject, focusObject, glm::vec3(1, 0, 0)));
									}
								} else if (angle < 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an obtuse angle, focus object cannot move in negative x
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i].focusPart == focusObject && contacts[i].otherPart == otherObject && contacts[i].direction == glm::vec3(1, 0, 0)) {
											alreadyExists = true;
										}
										if (contacts[i].focusPart == otherObject && contacts[i].otherPart == focusObject && contacts[i].direction == glm::vec3(-1, 0, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										contacts.push_back(ContactPair(focusObject, otherObject, glm::vec3(1, 0, 0)));
									}
									if (!alreadyExists2) {
										contacts.push_back(ContactPair(otherObject, focusObject, glm::vec3(-1, 0, 0)));
									}
								} else {
									//focus object is unblocked in x direction
								}
								angle = glm::dot(focusTriangleNormal, glm::vec3(0, 1, 0));
								if (angle > 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an acute angle, focus object cannot move in positive y
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i].focusPart == focusObject && contacts[i].otherPart == otherObject && contacts[i].direction == glm::vec3(0, -1, 0)) {
											alreadyExists = true;
										}
										if (contacts[i].focusPart == otherObject && contacts[i].otherPart == focusObject && contacts[i].direction == glm::vec3(0, 1, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										contacts.push_back(ContactPair(focusObject, otherObject, glm::vec3(0, -1, 0)));
									}
									if (!alreadyExists2) {
										contacts.push_back(ContactPair(otherObject, focusObject, glm::vec3(0, 1, 0)));
									}
								} else if (angle < 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an obtuse angle, focus object cannot move in negative y
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i].focusPart == focusObject && contacts[i].otherPart == otherObject && contacts[i].direction == glm::vec3(0, 1, 0)) {
											alreadyExists = true;
										}
										if (contacts[i].focusPart == otherObject && contacts[i].otherPart == focusObject && contacts[i].direction == glm::vec3(0, -1, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										contacts.push_back(ContactPair(focusObject, otherObject, glm::vec3(0, 1, 0)));
									}
									if (!alreadyExists2) {
										contacts.push_back(ContactPair(otherObject, focusObject, glm::vec3(0, -1, 0)));
									}
								} else {
									//focus object is unblocked in y direction
								}
								angle = glm::dot(focusTriangleNormal, glm::vec3(0, 0, 1));
								if (angle > 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an acute angle, focus object cannot move in positive z
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i].focusPart == focusObject && contacts[i].otherPart == otherObject && contacts[i].direction == glm::vec3(0, 0, -1)) {
											alreadyExists = true;
										}
										if (contacts[i].focusPart == otherObject && contacts[i].otherPart == focusObject && contacts[i].direction == glm::vec3(0, 0, 1)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										contacts.push_back(ContactPair(focusObject, otherObject, glm::vec3(0, 0, -1)));
									}
									if (!alreadyExists2) {
										contacts.push_back(ContactPair(otherObject, focusObject, glm::vec3(0, 0, 1)));
									}
								} else if (angle < 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an obtuse angle, focus object cannot move in negative z
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i].focusPart == focusObject && contacts[i].otherPart == otherObject && contacts[i].direction == glm::vec3(0, 0, 1)) {
											alreadyExists = true;
										}
										if (contacts[i].focusPart == otherObject && contacts[i].otherPart == focusObject && contacts[i].direction == glm::vec3(0, 0, -1)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										contacts.push_back(ContactPair(focusObject, otherObject, glm::vec3(0, 0, 1)));
									}
									if (!alreadyExists2) {
										contacts.push_back(ContactPair(otherObject, focusObject, glm::vec3(0, 0, -1)));
									}
								} else {
									//focus object is unblocked in z direction
								}
							}
						//}//if1
					//}//if2

				}
			}
		}
	}

	return contacts;
}

// ********************************************
// ********************************************
// ********************************************
// ********************************************
// ********************************************
// ********************************************
// ********************************************

std::vector<BlockingPair> ModelOperations::blocking(std::vector<UnpackedLists>& objects) {
	std::vector<BlockingPair> blockings;

	for (unsigned int focusObject = 0; focusObject < objects.size(); focusObject++) {
		for (unsigned int otherObject = focusObject + 1; otherObject < objects.size(); otherObject++) {
			for (unsigned int focusTriangleIndex = 0; focusTriangleIndex < objects[focusObject].verts.size(); focusTriangleIndex+=3) {
				for (unsigned int otherTriangleIndex = 0; otherTriangleIndex < objects[otherObject].verts.size(); otherTriangleIndex+=3) {

					Triangle3D focusTriangle(objects[focusObject].verts[focusTriangleIndex],
							                 objects[focusObject].verts[focusTriangleIndex + 1],
											 objects[focusObject].verts[focusTriangleIndex + 2], focusObject) ;


					Triangle3D otherTriangle(objects[otherObject].verts[otherTriangleIndex],
							                 objects[otherObject].verts[otherTriangleIndex + 1],
											 objects[otherObject].verts[otherTriangleIndex + 2], otherObject);

					//order is z, y, x
					std::vector<glm::vec2> intersectionPoints[3];
					bool intersect[3] = {false, false, false};

					for (unsigned int i = 0; i < 3; i++) {
						Triangle2D focusTriangleProj;
						Triangle2D otherTriangleProj;
						projectToPlane(i, focusTriangle, otherTriangle, focusTriangleProj, otherTriangleProj);

						int numIntersect = countIntersections(focusTriangleProj, otherTriangleProj, intersectionPoints[i]);
						int numPoints = countPointsInside(focusTriangleProj, otherTriangleProj, intersectionPoints[i]);

						// Triangles that only share one edge are not in contact
						if ((numIntersect >= 2 && numIntersect <= 6) || numPoints >= 1) {
							intersect[i] = true;
						}
					}

					if (std::abs(glm::dot(focusTriangle.getNormal(), otherTriangle.getNormal())) < 0.001) {
						intersect[0] = false;
						intersect[1] = false;
						intersect[2] = false;
					}

					if (glm::dot(focusTriangle.getNormal(), otherTriangle.getNormal()) > 0.999) {
						intersect[0] = false;
						intersect[1] = false;
						intersect[2] = false;
					}

					// Crazy logic happens
					if (intersect[0]) {
						reverseProject(intersectionPoints[0][0], intersectionPoints[0][1], intersectionPoints[0][2], glm::vec3(0, 0, 1), focusTriangle, otherTriangle, blockings);
					}

					if (intersect[1]) {
						reverseProject(intersectionPoints[1][0], intersectionPoints[1][1], intersectionPoints[1][2], glm::vec3(0, 1, 0), focusTriangle, otherTriangle, blockings);
					}

					if (intersect[2]) {
						reverseProject(intersectionPoints[2][0], intersectionPoints[2][1], intersectionPoints[2][2], glm::vec3(1, 0, 0), focusTriangle, otherTriangle, blockings);
					}

				}
			}
		}
	}
	return blockings;
}

void ModelOperations::projectToPlane(int i, Triangle3D tri1, Triangle3D tri2, Triangle2D& out1, Triangle2D& out2) {
	if (i == 0) {
		out1 = Triangle2D(glm::vec2(tri1.v0.x, tri1.v0.y),
				          glm::vec2(tri1.v1.x, tri1.v1.y),
						  glm::vec2(tri1.v2.x, tri1.v2.y));

		out2 = Triangle2D(glm::vec2(tri2.v0.x, tri2.v0.y),
				          glm::vec2(tri2.v1.x, tri2.v1.y),
						  glm::vec2(tri2.v2.x, tri2.v2.y));

	} else if (i == 1) {
		out1 = Triangle2D(glm::vec2(tri1.v0.x, tri1.v0.z),
				          glm::vec2(tri1.v1.x, tri1.v1.z),
						  glm::vec2(tri1.v2.x, tri1.v2.z));

		out2 = Triangle2D(glm::vec2(tri2.v0.x, tri2.v0.z),
				          glm::vec2(tri2.v1.x, tri2.v1.z),
						  glm::vec2(tri2.v2.x, tri2.v2.z));

	} else { //i == 2
		out1 = Triangle2D(glm::vec2(tri1.v0.y, tri1.v0.z),
				          glm::vec2(tri1.v1.y, tri1.v1.z),
						  glm::vec2(tri1.v2.y, tri1.v2.z));

		out2 = Triangle2D(glm::vec2(tri2.v0.y, tri2.v0.z),
				          glm::vec2(tri2.v1.y, tri2.v1.z),
						  glm::vec2(tri2.v2.y, tri2.v2.z));
	}
}

int ModelOperations::countIntersections(Triangle2D tri1, Triangle2D tri2, std::vector<glm::vec2>& intersectionPoints) {
	int numIntersects = 0;
	if (lineIntersect2D(tri1.v0, tri1.v1, tri2.v0, tri2.v1, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v0, tri1.v1, tri2.v0, tri2.v2, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v0, tri1.v1, tri2.v1, tri2.v2, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v0, tri1.v2, tri2.v0, tri2.v1, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v0, tri1.v2, tri2.v0, tri2.v2, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v0, tri1.v2, tri2.v1, tri2.v2, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v1, tri1.v2, tri2.v0, tri2.v1, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v1, tri1.v2, tri2.v0, tri2.v2, intersectionPoints)) {
		numIntersects++;
	}
	if (lineIntersect2D(tri1.v1, tri1.v2, tri2.v1, tri2.v2, intersectionPoints)) {
		numIntersects++;
	}
	return numIntersects;
}

int ModelOperations::countPointsInside(Triangle2D tri1, Triangle2D tri2, std::vector<glm::vec2>& intersectionPoints) {
	int numPoints = 0;
	if(pointInTriangle2D(tri1, tri2.v0)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri2.v0);
		}
		numPoints++;
	}
	if (pointInTriangle2D(tri1, tri2.v1)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri2.v1);
		}
		numPoints++;
	}
	if (pointInTriangle2D(tri1, tri2.v2)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri2.v2);
		}
		numPoints++;
	}
	if (pointInTriangle2D(tri2, tri1.v0)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri1.v0);
		}
		numPoints++;
	}
	if (pointInTriangle2D(tri2, tri1.v1)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri1.v1);
		}
		numPoints++;
	}
	if (pointInTriangle2D(tri2, tri1.v2)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri1.v2);
		}
		numPoints++;
	}
	return numPoints;
}

void ModelOperations::reverseProject(glm::vec2 intersectionPoint1, glm::vec2 intersectionPoint2, glm::vec2 intersectionPoint3, glm::vec3 projectionAxis, Triangle3D focusTriangle, Triangle3D otherTriangle, std::vector<BlockingPair>& blockings) {
	glm::vec2 center = ((1.0f/3.0f) * intersectionPoint1) + ((1.0f/3.0f) * intersectionPoint2) + ((1.0f/3.0f) * intersectionPoint3);
	glm::vec3 pa;
	if (projectionAxis.x == 1.0f) {
		pa = glm::vec3(0.0f, center.x, center.y);
	} else if (projectionAxis.y == 1.0f) {
		pa = glm::vec3(center.x, 0.0f, center.y);
	} else {
		pa = glm::vec3(center.x, center.y, 0.0f);
	}
	glm::vec3 barryPos;
	glm::vec3 intersectionPoint;

	bool a = glm::intersectLineTriangle(pa, projectionAxis, focusTriangle.v0, focusTriangle.v1, focusTriangle.v2, barryPos);
	intersectionPoint = focusTriangle.v0 * (1 - barryPos.y - barryPos.z) + focusTriangle.v1 * barryPos.y + focusTriangle.v2 * barryPos.z;
	float distanceFocus = glm::length(pa - intersectionPoint);
	if (glm::dot(pa - intersectionPoint, projectionAxis) < 0.0f) {
		distanceFocus*= -1;
	}

	bool b = glm::intersectLineTriangle(pa, projectionAxis, otherTriangle.v0, otherTriangle.v1, otherTriangle.v2, barryPos);
	intersectionPoint = otherTriangle.v0 * (1 - barryPos.y - barryPos.z) + otherTriangle.v1 * barryPos.y + otherTriangle.v2 * barryPos.z;
	float distanceOther = glm::length(pa - intersectionPoint);
	if (glm::dot(pa - intersectionPoint, projectionAxis) < 0.0f) {
		distanceOther*= -1;
	}

	float angle = glm::dot(focusTriangle.getNormal(), projectionAxis);
	if (angle < 0.0f) {
		if (distanceFocus >= (distanceOther - 0.001f)) {
			bool alreadyExists = false;
			bool alreadyExists2 = false;
			//vectors form an acute angle, focus object cannot move in positive y
			for (unsigned int i = 0; i < blockings.size(); i++) {
				if (blockings[i].focusPart == focusTriangle.object && blockings[i].otherPart == otherTriangle.object && blockings[i].direction == projectionAxis) {
					alreadyExists = true;
				}
				if (blockings[i].focusPart == otherTriangle.object && blockings[i].otherPart == focusTriangle.object && blockings[i].direction == -projectionAxis) {
					alreadyExists2 = true;
				}
			}
			if (!alreadyExists) {
				blockings.push_back(BlockingPair(focusTriangle.object, otherTriangle.object, projectionAxis));
			}
			if (!alreadyExists2) {
				blockings.push_back(BlockingPair(otherTriangle.object, focusTriangle.object, -projectionAxis));
			}
		}
	} else {
		if (distanceFocus <= (distanceOther + 0.001f)) {
			bool alreadyExists = false;
			bool alreadyExists2 = false;
			//vectors form an obtuse angle, focus object cannot move in negative y
			for (unsigned int i = 0; i < blockings.size(); i++) {
				if (blockings[i].focusPart == focusTriangle.object && blockings[i].otherPart == otherTriangle.object && blockings[i].direction == -projectionAxis) {
					alreadyExists = true;
				}
				if (blockings[i].focusPart == otherTriangle.object && blockings[i].otherPart == focusTriangle.object && blockings[i].direction == projectionAxis) {
					alreadyExists2 = true;
				}
			}
			if (!alreadyExists) {
				blockings.push_back(BlockingPair(focusTriangle.object, otherTriangle.object, -projectionAxis));
			}
			if (!alreadyExists2) {
				blockings.push_back(BlockingPair(otherTriangle.object, focusTriangle.object, projectionAxis));
			}
		}
	}
}

bool ModelOperations::getSimilarVertexIndex(
		PackedVertex & packed,
		std::map<PackedVertex,unsigned short> & VertexToOutIndex,
		unsigned short & result)
{
	std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()) {
		return false;
	}
	else {
		result = it->second;
		return true;
	}
}

void ModelOperations::indexVBO(
		std::vector<glm::vec3>& in_verts,
		std::vector<glm::vec2>& in_uvs,
		std::vector<glm::vec3>& in_normals,

		std::vector<unsigned short>& out_faces,
		std::vector<glm::vec3>& out_verts,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals)
{
	std::map<PackedVertex,unsigned short> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_verts.size(); i++ ){

		PackedVertex packed = {in_verts[i], in_uvs[i], in_normals[i]};

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex( packed, VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_faces.push_back( index );
		}else{ // If not, it needs to be added in the output data.
			out_verts.push_back( in_verts[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);
			unsigned short newindex = (unsigned short)out_verts.size() - 1;
			out_faces .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}

bool ModelOperations::lineIntersect3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
	//Code adapted from http://paulbourke.net/geometry/pointlineplane/lineline.c
	//judge if line (p1,p2) intersects with line(p3,p4)
	glm::vec3 p13,p43,p21;
	double d1343,d4321,d1321,d4343,d2121;
	double numer,denom;
	//Minimum distance below which lines are considered to intersect
	double EPS = 0.5;

	if (p1 == p3 && p2 == p4) {
		return true;
	}

	if (p1 == p4 && p2 == p3) {
		return true;
	}

	//Make sure the lines are not degenerate
	p13.x = p1.x - p3.x;
	p13.y = p1.y - p3.y;
	p13.z = p1.z - p3.z;
	p43.x = p4.x - p3.x;
	p43.y = p4.y - p3.y;
	p43.z = p4.z - p3.z;
	if (std::abs(p43.x) < EPS && std::abs(p43.y) < EPS && std::abs(p43.z) < EPS) {
		return false;
	}

	p21.x = p2.x - p1.x;
	p21.y = p2.y - p1.y;
	p21.z = p2.z - p1.z;
	if (std::abs(p21.x) < EPS && std::abs(p21.y) < EPS && std::abs(p21.z) < EPS) {
		return false;
	}

	d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
	d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
	d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
	d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
	d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

	denom = d2121 * d4343 - d4321 * d4321;
	if (std::abs(denom) < EPS) {
		return false;
	}

	numer = d1343 * d4321 - d1321 * d4343;

	double mua = numer / denom;
	double mub = (d1343 + d4321 * mua) / d4343;

	//TODO carefully consider this line
	if (mua <= 0 || mua >= 1 || mub <= 0 || mub >= 1) {
		return false;
	}

	glm::vec3 pa, pb;
	pa.x = p1.x + mua * p21.x;
	pa.y = p1.y + mua * p21.y;
	pa.z = p1.z + mua * p21.z;
	pb.x = p3.x + mub * p43.x;
	pb.y = p3.y + mub * p43.y;
	pb.z = p3.z + mub * p43.z;

	if (glm::length(pa - pb) < EPS) {
		return true;
	}

	return false;
}

bool ModelOperations::pointInLine(glm::vec2 l1, glm::vec2 l2, glm::vec2 v1) {
	  float EPS = 0.001f;

	bool interX = false;
	if (l1.x < l2.x) {
		interX = (interX || (v1.x > l1.x && v1.x < l2.x));
	}
	else if (l1.x > l2.x) {
		interX = (interX || (v1.x > l2.x && v1.x < l1.x));
	}
	else {
		interX = (abs(l1.x - l2.x) < 0.001f) && (abs(l2.x - v1.x) < 0.001f);
	}

	bool interY = false;
	if (l1.y < l2.y) {
		interY = (interY || (v1.y > l1.y && v1.y < l2.y));
	}
	else if (l1.x > l2.x) {
		interY = (interY || (v1.y > l2.y && v1.y < l1.y));
	}
	else {
		interY = (abs(l1.y - l2.y) < 0.001f) && (abs(l2.y - v1.y) < 0.001f);
	}
	return interX && interY;
}

bool ModelOperations::lineIntersect2D(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2 v4, std::vector<glm::vec2>& intersectionPoints) {

   double mua,mub;
   double denom,numera,numerb;
   float EPS = 0.001f;

   denom  = (v4.y-v3.y) * (v2.x-v1.x) - (v4.x-v3.x) * (v2.y-v1.y);
   numera = (v4.x-v3.x) * (v1.y-v3.y) - (v4.y-v3.y) * (v1.x-v3.x);
   numerb = (v2.x-v1.x) * (v1.y-v3.y) - (v2.y-v1.y) * (v1.x-v3.x);

   //Are the line coincident?
   if (std::abs(numera) < EPS && std::abs(numerb) < EPS && std::abs(denom) < EPS) {

	   // Crazy ass logic starts here
	   // Test if the lines segments are overlapping
	   bool v1In = pointInLine(v3, v4, v1);
	   bool v2In = pointInLine(v3, v4, v2);
	   bool v3In = pointInLine(v1, v2, v3);
	   bool v4In = pointInLine(v1, v2, v4);

	   glm::bvec2 a = glm::epsilonEqual(v1, v3, EPS);
	   glm::bvec2 b = glm::epsilonEqual(v2, v4, EPS);
	   glm::bvec2 c = glm::epsilonEqual(v1, v4, EPS);
	   glm::bvec2 d = glm::epsilonEqual(v2, v3, EPS);

	   bool same = ((a.x && a.y) && (b.x && b.y)) || ((c.x && c.y) && (d.x && d.y));

	   if ((v1In || v2In || v3In || v4In) || !same) {
		   return false;
	   }

	   if (intersectionPoints.size() < 3) {
		   intersectionPoints.push_back(0.5f * (v1 + v2));
	   }
	   return true;
   }

   //Are the line parallel?
   if (std::abs(denom) < EPS) {
      return false;
   }

   //Is the intersection along the the segments?
   mua = numera / denom;
   mub = numerb / denom;
   //TODO carefully consider this line
   if (mua <= 0 || mua >= 1 || mub <= 0 || mub >= 1) {
      return false;
   }

   if (intersectionPoints.size() < 3) {
	   intersectionPoints.push_back(glm::vec2(v1.x + mua * (v2.x - v1.x), v1.y + mua * (v2.y - v1.y)));
   }

   return true;
}

bool ModelOperations::pointInTriangle3D(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 p) {
	double s = 0.5 * glm::length(glm::cross(A - B, A - C));
	double s1 = 0.5 * glm::length(glm::cross(B - p, C - p));
	double s2 = 0.5 * glm::length(glm::cross(p - C, p - A));

	double a = s1 / s;
	double b = s2 / s;
	double c = 1.0 - a - b;

	if (a > 0.0 && a < 1.0 && b > 0.0 && b < 1.0 && c > 0.0 && c < 1.0) {
		return true;
	}
	return false;
}

bool ModelOperations::pointInTriangle2D(Triangle2D tri, glm::vec2 p) {
	double area = 0.5f * (-tri.v1.y * tri.v2.x + tri.v0.y * (-tri.v1.x + tri.v2.x) + tri.v0.x * (tri.v1.y - tri.v2.y) + tri.v1.x * tri.v2.y);
	int sign = (area < 0) ? -1 : 1;
    double s = (tri.v0.y * tri.v2.x - tri.v0.x * tri.v2.y + (tri.v2.y - tri.v0.y) * p.x + (tri.v0.x - tri.v2.x) * p.y) * sign;
    double t = (tri.v0.x * tri.v1.y - tri.v0.y * tri.v1.x + (tri.v0.y - tri.v1.y) * p.x + (tri.v1.x - tri.v0.x) * p.y) * sign;

    return s > 0 && t > 0 && ((s + t) < 2 * area * sign);
}
