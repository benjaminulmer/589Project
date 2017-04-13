#include "ModelOperations.h"

#include <stdio.h>
#include <iostream>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/intersect.hpp>

float EPS = 0.0001f;

// Creates blocking pair with two parts and a direction
BlockingPair::BlockingPair(unsigned int focusPart, unsigned int otherPart, glm::vec3 direction) : focusPart(focusPart), otherPart(otherPart), direction(direction) {}

// Splits an object into its separate parts
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

// Computes the set of all blockings for a set of objects
std::vector<BlockingPair> ModelOperations::blocking(std::vector<UnpackedLists>& objects) {
	std::vector<BlockingPair> blockings;

	// Compare all objects to each other
	for (unsigned int focusObject = 0; focusObject < objects.size(); focusObject++) {
		for (unsigned int otherObject = focusObject + 1; otherObject < objects.size(); otherObject++) {

			std::cout << "Computing blockings for " << focusObject << " and " << otherObject << " (total " << objects.size() << " objects)" << std::endl;

			// Compare all triangles in objects to each other
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

					// Test for intersections on each axis
					for (unsigned int i = 0; i < 3; i++) {
						Triangle2D focusTriangleProj;
						Triangle2D otherTriangleProj;
						projectToPlane(i, focusTriangle, otherTriangle, focusTriangleProj, otherTriangleProj);

						// If the area of a triangle is 0 it cannot intersect
						if (abs(focusTriangleProj.getArea()) < EPS || abs(otherTriangleProj.getArea()) < EPS) {
							continue;
						}

						// Count number of line intersections and number of points inside
						int numIntersect = countIntersections(focusTriangleProj, otherTriangleProj, intersectionPoints[i]);
						std::pair<int, int> numPoints = countPointsInside(focusTriangleProj, otherTriangleProj, intersectionPoints[i]);

						// One of these will be true if the triangles intersect
						if ((numIntersect >= 2) || numPoints.first >= 3 || numPoints.second >= 3) {
							intersect[i] = true;
						}
					}

					// Add blocking if there were intersections
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

// Projects 3D triangles to planar 2D triangles
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

// Counts line to line intersections between two 2D triangles
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

// Counts number of points tri1 has in tri2 and vis versa
std::pair<int, int> ModelOperations::countPointsInside(Triangle2D tri1, Triangle2D tri2, std::vector<glm::vec2>& intersectionPoints) {
	int numPointsA = 0;
	int numPointsB = 0;

	if(pointInTriangle2D(tri1, tri2.v0)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri2.v0);
		}
		numPointsA++;
	}
	if (pointInTriangle2D(tri1, tri2.v1)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri2.v1);
		}
		numPointsA++;
	}
	if (pointInTriangle2D(tri1, tri2.v2)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri2.v2);
		}
		numPointsA++;
	}
	if (pointInTriangle2D(tri2, tri1.v0)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri1.v0);
		}
		numPointsB++;
	}
	if (pointInTriangle2D(tri2, tri1.v1)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri1.v1);
		}
		numPointsB++;
	}
	if (pointInTriangle2D(tri2, tri1.v2)) {
		if (intersectionPoints.size() < 3) {
			intersectionPoints.push_back(tri1.v2);
		}
		numPointsB++;
	}
	return std::pair<int, int>(numPointsA, numPointsB);
}

// Reverse projects the 2D triangles to see which one is blocking which
void ModelOperations::reverseProject(glm::vec2 intersectionPoint1, glm::vec2 intersectionPoint2, glm::vec2 intersectionPoint3,
		                             glm::vec3 projectionAxis, Triangle3D focusTriangle, Triangle3D otherTriangle, std::vector<BlockingPair>& blockings) {

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

	// No intersection, we probably didn't want this case anyway
	if (!a || !b) {
		return;
	}

	float angle = glm::dot(focusTriangle.getNormal(), projectionAxis);
	if (angle < 0.0f) {
		if (distanceFocus >= (distanceOther - EPS)) {
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
		if (distanceFocus <= (distanceOther + EPS)) {
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

// Tests if a point is inside a line segment (2D)
bool ModelOperations::pointInLine2D(glm::vec2 l1, glm::vec2 l2, glm::vec2 v1) {
	bool interX = false;
	if (l1.x + EPS < l2.x) {
		interX = (interX || (v1.x > l1.x + EPS && v1.x + EPS < l2.x));
	}
	else if (l1.x > l2.x + EPS) {
		interX = (interX || (v1.x > l2.x + EPS && v1.x + EPS < l1.x));
	}
	else {
		interX = (abs(l1.x - l2.x) < EPS) && (abs(l2.x - v1.x) < EPS);
	}

	bool interY = false;
	if (l1.y + EPS < l2.y) {
		interY = (interY || (v1.y > l1.y + EPS && v1.y + EPS < l2.y));
	}
	else if (l1.x > l2.x + EPS) {
		interY = (interY || (v1.y > l2.y + EPS && v1.y + EPS < l1.y));
	}
	else {
		interY = (abs(l1.y - l2.y) < EPS) && (abs(l2.y - v1.y) < EPS);
	}
	return interX && interY;
}

// Tests if two line segments intersect in 2D
bool ModelOperations::lineIntersect2D(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2 v4, std::vector<glm::vec2>& intersectionPoints) {

   double mua,mub;
   double denom,numera,numerb;

   denom  = (v4.y-v3.y) * (v2.x-v1.x) - (v4.x-v3.x) * (v2.y-v1.y);
   numera = (v4.x-v3.x) * (v1.y-v3.y) - (v4.y-v3.y) * (v1.x-v3.x);
   numerb = (v2.x-v1.x) * (v1.y-v3.y) - (v2.y-v1.y) * (v1.x-v3.x);

   //Are the line coincident?
   if (std::abs(numera) < EPS && std::abs(numerb) < EPS && std::abs(denom) < EPS) {

	   // Crazy ass logic starts here
	   // Test if the lines segments are overlapping
	   bool v1In = pointInLine2D(v3, v4, v1);
	   bool v2In = pointInLine2D(v3, v4, v2);
	   bool v3In = pointInLine2D(v1, v2, v3);
	   bool v4In = pointInLine2D(v1, v2, v4);

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

// Tests if a point is inside of a triangle (2D)
bool ModelOperations::pointInTriangle2D(Triangle2D tri, glm::vec2 p) {
	double area = tri.getArea();
    double s = 1.f/(2.f*area) * (tri.v0.y * tri.v2.x - tri.v0.x * tri.v2.y + (tri.v2.y - tri.v0.y) * p.x + (tri.v0.x - tri.v2.x) * p.y);
    double t = 1.f/(2.f*area) * (tri.v0.x * tri.v1.y - tri.v0.y * tri.v1.x + (tri.v0.y - tri.v1.y) * p.x + (tri.v1.x - tri.v0.x) * p.y);

    return s > -EPS && t > -EPS && (1 - s - t) > -EPS;
}
