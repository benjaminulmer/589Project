#include "Renderable.h"
#include <iostream>

Renderable::Renderable() :
	vao(0), vertexBuffer(0),
	normalBuffer(0), uvBuffer(0),
	indexBuffer(0), textureID(0) {}

Renderable::~Renderable() {
	// Remove data from GPU
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &indexBuffer );
	glDeleteVertexArrays(1, &vao);
}
