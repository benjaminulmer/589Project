/*
 * ModelSplitter.h
 *
 *  Created on: Mar 9, 2017
 *      Author: jshall
 */

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Renderable;

class ModelSplitter {
public:
	ModelSplitter();
	virtual ~ModelSplitter();

	std::vector<Renderable*> split(Renderable* object);

};
