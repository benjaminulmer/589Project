#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <deque>
#include <map>
#include <cstdio>
#include <cstring>

#include "Renderable.h"

struct Triangle3D {
	Triangle3D(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int object) :
		v0(v1), v1(v2), v2(v3), object(object) {}

	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;

	unsigned int object;

	inline glm::vec3 getNormal() { return glm::normalize(glm::cross(v0 - v1, v2 - v1)); }
};

struct Triangle2D {
	Triangle2D() {}
    Triangle2D(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) :
        v0(v1), v1(v2), v2(v3) {}

	float getArea() { return 0.5f * (-v1.y * v2.x + v0.y * (-v1.x + v2.x) + v0.x * (v1.y - v2.y) + v1.x * v2.y); }

    glm::vec2 v0;
    glm::vec2 v1;
    glm::vec2 v2;
};

struct PackedVertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};

struct BlockingPair {
	BlockingPair(unsigned int focusPart, unsigned int otherPart, glm::vec3 direction);
	unsigned int focusPart;
	unsigned int otherPart;
	glm::vec3 direction;
};

class ModelOperations {

public:
	static std::vector<UnpackedLists> split(IndexedLists& object);
	static std::vector<BlockingPair> blocking(std::vector<UnpackedLists>& objects);

	static void projectToPlane(int i, Triangle3D tri1, Triangle3D tri2, Triangle2D& out1, Triangle2D& out2);
	static int countIntersections(Triangle2D tri1, Triangle2D tri2, std::vector<glm::vec2>& intersectionPoints);
	static std::pair<int, int> countPointsInside(Triangle2D tri1, Triangle2D tri2, std::vector<glm::vec2>& intersectionPoints);

	static void reverseProject(glm::vec2 intersectionPoint1, glm::vec2 intersectionPoint2, glm::vec2 intersectionPoint3, glm::vec3 projectionAxis,
			                   Triangle3D focusTriangle, Triangle3D otherTriangle, std::vector<BlockingPair>& blockings);

	static bool lineIntersect2D(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2 v4, std::vector<glm::vec2>& intersectionPoints);

	static bool pointInLine(glm::vec2 l1, glm::vec2 l2, glm::vec2 v1);

	static bool pointInTriangle2D(Triangle2D tri, glm::vec2 p);

	static bool getSimilarVertexIndex(
		PackedVertex & packed,
		std::map<PackedVertex,unsigned short> & VertexToOutIndex,
		unsigned short & result);

	static void indexVBO(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,
		std::vector<unsigned short> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals);

};
