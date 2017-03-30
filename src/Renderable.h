#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

struct IndexedLists {
	std::vector<GLushort> vertIndices;
	std::vector<GLushort> normalIndices;
	std::vector<GLushort> uvIndices;

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

struct UnpackedLists {
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

class Renderable {

public:
	Renderable();
	virtual ~Renderable();

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<GLushort> faces;

	GLuint vao;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint uvBuffer;
	GLuint indexBuffer;
	GLuint textureID;
	glm::vec3 colour;

	glm::vec3 getDimensions();
	glm::vec3 getPosition();

private:
	glm::vec3 dimensions;
	glm::vec3 position;
	bool boundingBoxComputed;

	void computeBoundingBox();
};

