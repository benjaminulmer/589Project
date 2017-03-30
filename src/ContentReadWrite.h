#pragma once

#include <rapidjson/document.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>

#include "ModelSplitter.h"
#include "ExplosionGraph.h"
#include "Renderable.h"

struct PackedVertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};

class ContentReadWrite {

public:
	static std::pair<std::vector<Renderable*>, std::vector<BlockingPair>> readRenderable(std::string modelFile);
	static ExplosionGraph* readExplosionGraph(std::string graphFile);
	static void writeExplosionGraph(ExplosionGraph* graph, std::string graphFile);

private:
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

	static bool loadOBJ(const char* path, IndexedLists& r);
};

