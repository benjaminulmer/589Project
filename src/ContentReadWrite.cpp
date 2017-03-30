#include "ContentReadWrite.h"

// Create renderable from obj file
std::pair<std::vector<Renderable*>, std::vector<BlockingPair>>  ContentReadWrite::readRenderable(std::string modelFile) {
	std::cout << "Creating renderables" << std::endl;

	// Read data in from obj file
	IndexedLists obj;
	ContentReadWrite::loadOBJ(modelFile.c_str(), obj);

	// Split obj into separate parts
	std::vector<UnpackedLists> split = ModelOperations::split(obj);

	// Compute contacts and blocking
	std::vector<BlockingPair> blockings = ModelOperations::contactsAndBlocking(split);

	// Convert indexed lists into renderables
	std::vector<Renderable*> renderables(split.size());
	for (unsigned int i = 0; i < split.size(); i++) {
		renderables[i] = new Renderable();
		ModelOperations::indexVBO(split[i].verts, split[i].uvs, split[i].normals, renderables[i]->faces, renderables[i]->verts, renderables[i]->uvs, renderables[i]->normals);
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
