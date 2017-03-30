#include "ContentReadWrite.h"

// Create renderable from obj file
std::pair<std::vector<Renderable*>, std::vector<BlockingPair>>  ContentReadWrite::readRenderable(std::string modelFile) {
	std::cout << "Creating renderables" << std::endl;

	// Read data in from obj file
	IndexedLists obj;
	ContentReadWrite::loadOBJ(modelFile.c_str(), obj);

	// Split obj into separate parts
	std::vector<IndexedLists> split = ModelSplitter::split(obj);
	for (unsigned int i = 0; i < split.size(); i++) {
		//output[i]->id = i;
	}

	// Compute contacts and blocking
	std::vector<BlockingPair> blockings = ModelSplitter::contactsAndBlocking(split);

	// Convert indexed lists into renderables
	std::vector<Renderable*> renderables(split.size());
	for (unsigned int i = 0; i < split.size(); i++) {
		std::vector<unsigned short> indices;
		std::vector<glm::vec3> indexedVertices;
		std::vector<glm::vec2> indexedUvs;
		std::vector<glm::vec3> indexedNormals;
		ContentReadWrite::indexVBO(split[i].verts, split[i].uvs, split[i].normals, indices, indexedVertices, indexedUvs, indexedNormals);
		renderables[i] = new Renderable();
		renderables[i]->verts = indexedVertices;
		renderables[i]->uvs = indexedUvs;
		renderables[i]->normals = indexedNormals;
		renderables[i]->faces = indices;
	}

	return std::pair<std::vector<Renderable*>, std::vector<BlockingPair>>(renderables, blockings);
}

// Reads in explosion graph from file
ExplosionGraph* ContentReadWrite::readExplosionGraph(std::string graphFile) {
	std::vector<glm::vec3> points;

	std::ifstream file(graphFile);
	if (!file.is_open()) {
		std::cout << "Could not open file" << std::endl;
		return nullptr;
	}

	// Get file length
	file.seekg (0, file.end);
	int length = file.tellg();
	file.seekg (0, file.beg);

	// Read file into buffer and parse
	char* buffer = new char[length + 1];
	file.read(buffer, length);
	file.close();
	buffer[length] = 0;

	rapidjson::Document d;
	d.Parse(buffer);
	delete[] buffer;

	// Return if no points member
	if (!d.HasMember("nodes")) {
		std::cout << "Missing nodes" << std::endl;
		return nullptr;
	}

	// Return if no points member
	if (!d.HasMember("graph")) {
		std::cout << "Missing graph" << std::endl;
		return nullptr;
	}

	for (rapidjson::SizeType i = 0; i < d["points"].Size(); i++) {

		// Return if entry not valid
		rapidjson::Value& entry = d["points"][i];
		if (!entry.HasMember("x") || !entry.HasMember("y") || !entry.HasMember("z")) {
			continue;
		}

		// Read entry and add to points
		glm::vec3 point;
		point.x = (float)entry["x"].GetDouble();
		point.y = (float)entry["y"].GetDouble();
		point.z = (float)entry["z"].GetDouble();
		points.push_back(point);
	}

	return 0;
}

// Writes explosion graph to file
void ContentReadWrite::writeExplosionGraph(ExplosionGraph* graph, std::string graphFile) {

}

bool ContentReadWrite::getSimilarVertexIndex( 
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

void ContentReadWrite::indexVBO(
		std::vector<glm::vec3>& in_verts,
		std::vector<glm::vec2>& in_uvs,
		std::vector<glm::vec3>& in_normals,

		std::vector<unsigned short>& out_faces,
		std::vector<glm::vec3>& out_verts,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals)
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

bool ContentReadWrite::loadOBJ(const char* path, IndexedLists& r) {
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
			r.verts.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; 
			r.uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			r.normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );

			r.vertIndices.push_back(vertexIndex[0]-1);
			r.vertIndices.push_back(vertexIndex[1]-1);
			r.vertIndices.push_back(vertexIndex[2]-1);
			r.uvIndices.push_back(uvIndex[0]-1);
			r.uvIndices.push_back(uvIndex[1]-1);
			r.uvIndices.push_back(uvIndex[2]-1);
			r.normalIndices.push_back(normalIndex[0]-1);
			r.normalIndices.push_back(normalIndex[1]-1);
			r.normalIndices.push_back(normalIndex[2]-1);

		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}
	
	fclose(file);
	return true;
}
