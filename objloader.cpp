#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <glm/glm.hpp>
#include "objloader.hpp"

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec3> & out_normals) 
{
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_normals;

	std::ifstream file(path);
	if (!file.is_open()) {
		printf("Impossible to open the file !\n");
		getchar();
		return false;
	}

	while (1) {

		std::string lineHeader;
		file >> lineHeader;
		if (file.eof())
			break; 

		if (lineHeader == "v") {
			glm::vec3 vertex;
			file >> vertex.x >> vertex.y >> vertex.z;
			temp_vertices.push_back(vertex);
		}
		else if (lineHeader == "vn") {
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (lineHeader == "f") {
			std::string vertex1, vertex2, vertex3;
			char ch;
			unsigned int vertexIndex[3], normalIndex[3];
			file >> vertexIndex[0] >> ch >> ch >> normalIndex[0] >> vertexIndex[1] >> ch >> ch >> normalIndex[1] >> vertexIndex[2] >> ch >> ch >> normalIndex[2];
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			std::string DumpBuffer;
			std::getline(file, DumpBuffer);
		}

	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		unsigned int vertexIndex = vertexIndices[i];
		unsigned int normalIndex = normalIndices[i];

		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		out_vertices.push_back(vertex);
		out_normals.push_back(normal);
	}

	return true;
}
