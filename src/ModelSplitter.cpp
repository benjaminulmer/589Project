#include "ModelSplitter.h"

#include <stdio.h>

// Constructor for a block, always needs part and direction
Blocking::Blocking(Renderable* focusPart, Renderable* otherPart, glm::vec3 direction) : focusPart(focusPart), otherPart(otherPart), direction(direction) {}

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

std::vector<Blocking*> ModelSplitter::contactsAndBlocking(std::vector<Renderable*> objects) {
	std::vector<Blocking*> contacts;

	for (unsigned int focusObject = 0; focusObject < objects.size(); focusObject++) {
		for (unsigned int otherObject = focusObject + 1; otherObject < objects.size(); otherObject++) {
			for (unsigned int focusTriangle = 0; focusTriangle < objects[focusObject]->verts.size(); focusTriangle+=3) {
				for (unsigned int otherTriangle = 0; otherTriangle < objects[otherObject]->verts.size(); otherTriangle+=3) {
					glm::vec3 focusTriangleVerts[3];
					focusTriangleVerts[0] = objects[focusObject]->verts[focusTriangle];
					focusTriangleVerts[1] = objects[focusObject]->verts[focusTriangle + 1];
					focusTriangleVerts[2] = objects[focusObject]->verts[focusTriangle + 2];

					glm::vec3 otherTriangleVerts[3];
					otherTriangleVerts[0] = objects[otherObject]->verts[otherTriangle];
					otherTriangleVerts[1] = objects[otherObject]->verts[otherTriangle + 1];
					otherTriangleVerts[2] = objects[otherObject]->verts[otherTriangle + 2];

					//TODO Compute blocking information here

					//Compute face normals of the two triangles
					//Follow right hand rule
					glm::vec3 focusTriangleNormal = glm::normalize(glm::cross(focusTriangleVerts[0] - focusTriangleVerts[1], focusTriangleVerts[2] - focusTriangleVerts[1]));

					glm::vec3 otherTriangleNormal = glm::normalize(glm::cross(otherTriangleVerts[0] - otherTriangleVerts[1], otherTriangleVerts[2] - otherTriangleVerts[1]));
					int debug = 0;
					//Decide if the normalized normals are the same
					if (focusTriangleNormal.x == -otherTriangleNormal.x && focusTriangleNormal.y == -otherTriangleNormal.y && focusTriangleNormal.z == -otherTriangleNormal.z) {
						glm::vec3 offset = focusTriangleVerts[0] - otherTriangleVerts[0];
						//Test if the triangles are in the same plane
						if (glm::dot(focusTriangleNormal, offset) == 0.0) {
							//Determine if the triangles intersect
							bool intersect = false;
							int numIntersect = 0;
							if (lineIntersect(focusTriangleVerts[0],focusTriangleVerts[1],otherTriangleVerts[0],otherTriangleVerts[1])) {
								numIntersect++;
								//intersect = true;
								debug = 1;
							}
							if (lineIntersect(focusTriangleVerts[0],focusTriangleVerts[1],otherTriangleVerts[0],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
								debug = 2;
							}
							if (lineIntersect(focusTriangleVerts[0],focusTriangleVerts[1],otherTriangleVerts[1],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
								debug = 3;
							}
							if (lineIntersect(focusTriangleVerts[0],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[1])) {
								numIntersect++;
								//intersect = true;
								debug = 4;
							}
							if (lineIntersect(focusTriangleVerts[0],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
								debug = 5;
							}
							if (lineIntersect(focusTriangleVerts[0],focusTriangleVerts[2],otherTriangleVerts[1],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
								debug = 6;
							}
							if (lineIntersect(focusTriangleVerts[1],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[1])) {
								numIntersect++;
								//intersect = true;
								debug = 7;
							}
							if (lineIntersect(focusTriangleVerts[1],focusTriangleVerts[2],otherTriangleVerts[0],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
								debug = 8;
							}
							if (lineIntersect(focusTriangleVerts[1],focusTriangleVerts[2],otherTriangleVerts[1],otherTriangleVerts[2])) {
								numIntersect++;
								//intersect = true;
								debug = 9;
							}
							//Triangles that only share one edge may not be in contact
							if (numIntersect > 1) {
								intersect = true;
							}
							bool inTri = true;
							inTri = inTri && pointInTriangle(focusTriangleVerts[0],focusTriangleVerts[1],focusTriangleVerts[2], otherTriangleVerts[0]);
							inTri = inTri && pointInTriangle(focusTriangleVerts[0],focusTriangleVerts[1],focusTriangleVerts[2], otherTriangleVerts[1]);
							inTri = inTri && pointInTriangle(focusTriangleVerts[0],focusTriangleVerts[1],focusTriangleVerts[2], otherTriangleVerts[2]);
							if (inTri) {
								intersect = true;
								debug = 10;
							}
							inTri = true;
							inTri = inTri && pointInTriangle(otherTriangleVerts[0],otherTriangleVerts[1],otherTriangleVerts[2], focusTriangleVerts[0]);
							inTri = inTri && pointInTriangle(otherTriangleVerts[0],otherTriangleVerts[1],otherTriangleVerts[2], focusTriangleVerts[1]);
							inTri = inTri && pointInTriangle(otherTriangleVerts[0],otherTriangleVerts[1],otherTriangleVerts[2], focusTriangleVerts[2]);
							if (inTri) {
								intersect = true;
								debug = 11;
							}
							if (intersect) {
								//Determine blocking direction(s)
								float angle = glm::dot(focusTriangleNormal, glm::vec3(1, 0, 0));
								if (angle > 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an acute angle, focus object cannot move in positive x, other cannot move in -x
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i]->focusPart == objects[focusObject] && contacts[i]->otherPart == objects[otherObject] && contacts[i]->direction == glm::vec3(1, 0, 0)) {
											alreadyExists = true;
										}
										if (contacts[i]->focusPart == objects[otherObject] && contacts[i]->otherPart == objects[focusObject] && contacts[i]->direction == glm::vec3(-1, 0, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										Blocking* block = new Blocking(objects[focusObject], objects[otherObject], glm::vec3(1, 0, 0));
										contacts.push_back(block);
									}
									if (!alreadyExists2) {
										Blocking* block = new Blocking(objects[otherObject], objects[focusObject], glm::vec3(-1, 0, 0));
										contacts.push_back(block);
									}
								} else if (angle < 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an obtuse angle, focus object cannot move in negative x
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i]->focusPart == objects[focusObject] && contacts[i]->otherPart == objects[otherObject] && contacts[i]->direction == glm::vec3(-1, 0, 0)) {
											alreadyExists = true;
										}
										if (contacts[i]->focusPart == objects[otherObject] && contacts[i]->otherPart == objects[focusObject] && contacts[i]->direction == glm::vec3(1, 0, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										Blocking* block = new Blocking(objects[focusObject], objects[otherObject], glm::vec3(-1, 0, 0));
										contacts.push_back(block);
									}
									if (!alreadyExists2) {
										Blocking* block = new Blocking(objects[otherObject], objects[focusObject], glm::vec3(1, 0, 0));
										contacts.push_back(block);
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
										if (contacts[i]->focusPart == objects[focusObject] && contacts[i]->otherPart == objects[otherObject] && contacts[i]->direction == glm::vec3(0, 1, 0)) {
											alreadyExists = true;
										}
										if (contacts[i]->focusPart == objects[otherObject] && contacts[i]->otherPart == objects[focusObject] && contacts[i]->direction == glm::vec3(0, -1, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										Blocking* block = new Blocking(objects[focusObject], objects[otherObject], glm::vec3(0, 1, 0));
										contacts.push_back(block);
									}
									if (!alreadyExists2) {
										Blocking* block = new Blocking(objects[otherObject], objects[focusObject], glm::vec3(0, -1, 0));
										contacts.push_back(block);
									}
								} else if (angle < 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an obtuse angle, focus object cannot move in negative y
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i]->focusPart == objects[focusObject] && contacts[i]->otherPart == objects[otherObject] && contacts[i]->direction == glm::vec3(0, -1, 0)) {
											alreadyExists = true;
										}
										if (contacts[i]->focusPart == objects[otherObject] && contacts[i]->otherPart == objects[focusObject] && contacts[i]->direction == glm::vec3(0, 1, 0)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										Blocking* block = new Blocking(objects[focusObject], objects[otherObject], glm::vec3(0, -1, 0));
										contacts.push_back(block);
									}
									if (!alreadyExists2) {
										Blocking* block = new Blocking(objects[otherObject], objects[focusObject], glm::vec3(0, 1, 0));
										contacts.push_back(block);
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
										if (contacts[i]->focusPart == objects[focusObject] && contacts[i]->otherPart == objects[otherObject] && contacts[i]->direction == glm::vec3(0, 0, 1)) {
											alreadyExists = true;
										}
										if (contacts[i]->focusPart == objects[otherObject] && contacts[i]->otherPart == objects[focusObject] && contacts[i]->direction == glm::vec3(0, 0, -1)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										Blocking* block = new Blocking(objects[focusObject], objects[otherObject], glm::vec3(0, 0, 1));
										contacts.push_back(block);
									}
									if (!alreadyExists2) {
										Blocking* block = new Blocking(objects[otherObject], objects[focusObject], glm::vec3(0, 0, -1));
										contacts.push_back(block);
									}
								} else if (angle < 0.0) {
									bool alreadyExists = false;
									bool alreadyExists2 = false;
									//vectors form an obtuse angle, focus object cannot move in negative z
									for (unsigned int i = 0; i < contacts.size(); i++) {
										if (contacts[i]->focusPart == objects[focusObject] && contacts[i]->otherPart == objects[otherObject] && contacts[i]->direction == glm::vec3(0, 0, -1)) {
											alreadyExists = true;
										}
										if (contacts[i]->focusPart == objects[otherObject] && contacts[i]->otherPart == objects[focusObject] && contacts[i]->direction == glm::vec3(0, 0, 1)) {
											alreadyExists2 = true;
										}
									}
									if (!alreadyExists) {
										Blocking* block = new Blocking(objects[focusObject], objects[otherObject], glm::vec3(0, 0, -1));
										contacts.push_back(block);
									}
									if (!alreadyExists2) {
										Blocking* block = new Blocking(objects[otherObject], objects[focusObject], glm::vec3(0, 0, 1));
										contacts.push_back(block);
									}
								} else {
									//focus object is unblocked in z direction
								}
							}
						}
					}

				}
			}
		}
	}

	for (unsigned int i = 0; i < contacts.size(); i++) {
		printf("focusObject = %d otherObject = %d dir = %f, %f, %f\n", contacts[i]->focusPart->id, contacts[i]->otherPart->id, contacts[i]->direction.x, contacts[i]->direction.y, contacts[i]->direction.z);
	}
	printf("numContacts = %d\n", contacts.size());

	return contacts;
}

bool ModelSplitter::lineIntersect(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
	//Code adapted from http://paulbourke.net/geometry/pointlineplane/lineline.c
	//judge if line (p1,p2) intersects with line(p3,p4)
	glm::vec3 p13,p43,p21;
	double d1343,d4321,d1321,d4343,d2121;
	double numer,denom;
	//Minimum distance below which lines are considered to intersect
	double EPS = 0.01;

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

bool ModelSplitter::pointInTriangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 p) {
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
