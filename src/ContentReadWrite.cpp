#include "ContentReadWrite.h"

// Create renderable from obj file
std::vector<UnpackedLists> ContentReadWrite::partsFromObj(std::string modelFile) {
	IndexedLists obj;
	ContentReadWrite::loadOBJ(modelFile.c_str(), obj);

	return ModelOperations::split(obj);
}

// Reads in explosion graph from file
rapidjson::Document ContentReadWrite::readExplosionGraph(std::string graphFile) {

	// Open file
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

	// Create JSON document
	rapidjson::Document d;
	rapidjson::ParseResult ok = d.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer);

	if (!ok) {
		rapidjson::ParseErrorCode error = ok.Code();
		std::cout << "error parsing JSON file: " << error << std::endl;
	}
	delete[] buffer;

	return d;
}

// Writes explosion graph to file
void ContentReadWrite::writeExplosionGraph(rapidjson::Document& d, std::string graphFile) {

	// Open file
	std::ofstream file(graphFile);
	if (!file.is_open()) {
		std::cout << "Could not open file" << std::endl;
		return;
	}

	// Pretty writer for JSON document
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);

	// Get data and size then write to file
	const char* output = buffer.GetString();
	long size = buffer.GetSize();

	file.write(output, size);
}

// Load obj file from path into indexed list data structure
bool ContentReadWrite::loadOBJ(const char* path, IndexedLists& r) {
	printf("Loading OBJ file %s\n", path);

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
