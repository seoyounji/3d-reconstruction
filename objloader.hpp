#pragma once
#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <glm/common.hpp>

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec3> & out_normals
);

#endif