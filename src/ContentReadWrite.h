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
	static std::vector<UnpackedLists> partsFromObj(std::string modelFile);

	static rapidjson::Document readExplosionGraph(std::string graphFile);
	static void writeExplosionGraph(rapidjson::Document d, std::string graphFile);

private:
	static bool loadOBJ(const char* path, IndexedLists& r);
};

