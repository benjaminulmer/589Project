#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

class Renderable {

public:
	Renderable();
	virtual ~Renderable();

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> drawVerts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<GLushort> faces;
	std::vector<GLushort> drawFaces;

	GLuint vao;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint uvBuffer;
	GLuint indexBuffer;
	GLuint textureID;
	glm::vec4 colour;

	glm::vec3 getDimensions();
	glm::vec3 getPosition();

private:
	glm::vec3 dimensions;
	glm::vec3 position;
	bool boundingBoxComputed;

	void computeBoundingBox();
};

