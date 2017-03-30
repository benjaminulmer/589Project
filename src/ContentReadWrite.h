#pragma once

#include <rapidjson/document.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>

#include <vector>
#include <string>

#include "ExplosionGraph.h"
#include "ModelOperations.h"
#include "Renderable.h"

class ContentReadWrite {

public:
	static std::pair<std::vector<Renderable*>, std::vector<BlockingPair>> readRenderable(std::string modelFile);
	static ExplosionGraph* readExplosionGraph(std::string graphFile);
	static void writeExplosionGraph(ExplosionGraph* graph, std::string graphFile);

private:
	static bool loadOBJ(const char* path, IndexedLists& r);
};

