#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <deque>
#include <map>
#include <cstdio>
#include <cstring>

#include "Renderable.h"

struct PackedVertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};

struct ContactPair {
	ContactPair(unsigned int focusPart, unsigned int otherPart, glm::vec3 direction);
	unsigned int focusPart;
	unsigned int otherPart;
	glm::vec3 direction;
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
	static std::vector<ContactPair> contacts(std::vector<UnpackedLists>& objects);
	static std::vector<BlockingPair> blocking(std::vector<UnpackedLists>& objects);
	static bool lineIntersect2D(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2 v4, glm::vec2 intersectionPoint);
	static bool lineIntersect3D(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
	static bool pointInTriangle3D(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 p);

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
