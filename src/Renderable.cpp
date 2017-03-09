#include "Renderable.h"

Renderable::Renderable() :
	vao(0), vertexBuffer(0), normalBuffer(0), uvBuffer(0),
	indexBuffer(0), textureID(0), boundingBoxComputed(false) {}

Renderable::~Renderable() {
	// Remove data from GPU
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &vao);
}

// Returns dimension of objects bounding box
glm::vec3 Renderable::getDimensions() {
	if (!boundingBoxComputed) {
		computeBoundingBox();
	}
	return dimensions;
}

// Returns centre of objects bounding box
glm::vec3 Renderable::getPosition() {
	if (!boundingBoxComputed) {
		computeBoundingBox();
	}
	return position;
}

// Computed objects bounding box stored as dimensions and centre of box
void Renderable::computeBoundingBox() {
	// compute here
}
