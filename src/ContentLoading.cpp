#include "ContentLoading.h"

// Create renderable from obj file
std::vector<Renderable*> ContentLoading::createRenderables(std::string modelFile) {

	// Read data in from obj file
	Renderable * r = new Renderable();
	ContentLoading::loadOBJ(modelFile.c_str(), r);

	ModelSplitter splitter = ModelSplitter();
	std::vector<Renderable*> output = splitter.split(r);

	for (unsigned int i = 0; i < output.size(); i++) {
		std::vector<unsigned short> indices;
		std::vector<glm::vec3> indexedVertices;
		std::vector<glm::vec2> indexedUvs;
		std::vector<glm::vec3> indexedNormals;
		ContentLoading::indexVBO(output[i]->verts, output[i]->uvs, output[i]->normals, indices, indexedVertices, indexedUvs, indexedNormals);
		output[i]->verts = indexedVertices;
		output[i]->uvs = indexedUvs;
		output[i]->normals = indexedNormals;
		output[i]->faces = indices;
	}

	return output;
}

bool ContentLoading::getSimilarVertexIndex( 
		PackedVertex & packed,
		std::map<PackedVertex,unsigned short> & VertexToOutIndex,
		unsigned short & result)
{
	std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()) {
		return false;
	}
	else {
		result = it->second;
		return true;
	}
}

void ContentLoading::indexVBO(
		std::vector<glm::vec3> & in_verts,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,

		std::vector<unsigned short> & out_faces,
		std::vector<glm::vec3> & out_verts,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals)
{
	std::map<PackedVertex,unsigned short> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_verts.size(); i++ ){

		PackedVertex packed = {in_verts[i], in_uvs[i], in_normals[i]};
		
		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex( packed, VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_faces.push_back( index );
		}else{ // If not, it needs to be added in the output data.
			out_verts.push_back( in_verts[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);
			unsigned short newindex = (unsigned short)out_verts.size() - 1;
			out_faces .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}

bool ContentLoading::loadOBJ(const char* path, Renderable* r) {
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
			r->verts.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; 
			r->uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			r->normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );

			r->vertIndices.push_back(vertexIndex[0]-1);
			r->vertIndices.push_back(vertexIndex[1]-1);
			r->vertIndices.push_back(vertexIndex[2]-1);
			r->uvIndices.push_back(uvIndex[0]-1);
			r->uvIndices.push_back(uvIndex[1]-1);
			r->uvIndices.push_back(uvIndex[2]-1);
			r->normalIndices.push_back(normalIndex[0]-1);
			r->normalIndices.push_back(normalIndex[1]-1);
			r->normalIndices.push_back(normalIndex[2]-1);

		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}
	
	fclose(file);
	return true;
}
