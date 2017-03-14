#include "ContentLoading.h"

#include "ModelSplitter.h"

#include <stdio.h>

// Create renderable from obj file
std::vector<Renderable*> ContentLoading::createRenderable(std::string modelFile) {
	Renderable * r = new Renderable();
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<GLushort> faces;
	std::vector<GLushort> normalIndices;
	std::vector<GLushort> uvIndices;

	ContentLoading::loadOBJ(modelFile.c_str(), verts, uvs, normals, faces, normalIndices, uvIndices);

	r->rawVerts = verts;
	r->uvs = uvs;
	r->normals = normals;
	r->faces = faces;
	r->normalIndices = normalIndices;
	r->uvIndices = uvIndices;

	ModelSplitter splitter = ModelSplitter();
	std::vector<Renderable*> output = splitter.split(r);

	printf("number models = %d\n", output.size());

	/*for (unsigned int i = 0; i < output.size(); i++) {
		printf("i = %d\n", i);
		for (unsigned int j = 0; j < output[i]->rawVerts.size(); j++) {
			printf("x = %f y = %f z = %f\n", output[i]->rawVerts[j].x, output[i]->rawVerts[j].y, output[i]->rawVerts[j].z);
		}
		for (unsigned int j = 0; j < output[i]->faces.size(); j+=3) {
			printf("v1 = %d v2 = %d v3 = %d\n", output[i]->faces[j], output[i]->faces[j + 1], output[i]->faces[j + 2]);
		}
	}*/

	for (unsigned int i = 0; i < output.size(); i++) {
		std::vector<unsigned short> indices;
		std::vector<glm::vec3> indexed_vertices;
		std::vector<glm::vec2> indexed_uvs;
		std::vector<glm::vec3> indexed_normals;
		ContentLoading::indexVBO(output[i]->drawVerts, output[i]->uvs, output[i]->normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
		output[i]->drawVerts = indexed_vertices;
		output[i]->uvs = indexed_uvs;
		output[i]->normals = indexed_normals;
		output[i]->drawFaces = indices;
	}

	return output;

	//return r;
}

bool ContentLoading::getSimilarVertexIndex_fast( 
		PackedVertex & packed,
		std::map<PackedVertex,unsigned short> & VertexToOutIndex,
		unsigned short & result)
{
	std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if ( it == VertexToOutIndex.end() ){
		return false;
	}else{
		result = it->second;
		return true;
	}
}

void ContentLoading::indexVBO(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,

		std::vector<unsigned short> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals)
{
	std::map<PackedVertex,unsigned short> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){

		PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};
		

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}

bool ContentLoading::loadOBJ(
		const char * path,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals,
		std::vector<GLushort> & out_faces,
		std::vector<GLushort> & out_normalIndices,
		std::vector<GLushort> & out_uvIndices)
{
	printf("Loading OBJ file %s...\n", path);

	FILE * file; 
	file = fopen(path, "r");
	if( file == NULL ){
		printf("Cannot open file. Check path.");
		getchar();
		return false;
	}

	while(true){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			out_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; 
			out_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			out_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );

			out_faces.push_back(vertexIndex[0]-1);
			out_faces.push_back(vertexIndex[1]-1);
			out_faces.push_back(vertexIndex[2]-1);
			out_uvIndices.push_back(uvIndex[0]-1);
			out_uvIndices.push_back(uvIndex[1]-1);
			out_uvIndices.push_back(uvIndex[2]-1);
			out_normalIndices.push_back(normalIndex[0]-1);
			out_normalIndices.push_back(normalIndex[1]-1);
			out_normalIndices.push_back(normalIndex[2]-1);

		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	//Split the object here

	// For each vertex of each triangle
	/*for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}*/
	fclose(file);
	return true;
}
