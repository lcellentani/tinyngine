#pragma once

#include <cstdint>
#include <vector>

namespace tinyngine
{

struct CubeGeometry {
	uint32_t numVertices;
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texcoords;

	uint32_t numIndices;
	std::vector<uint32_t> indices;
};

/*struct ObjGeometrySOA {
	uint32_t numVertices;
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texcoords;

	uint32_t numIndices;
	std::vector<uint32_t> indices;
};*/

void GenerateCube(float scale, CubeGeometry& result);
void LoadObj(const  char * filename, bool triangulate = true);

} // namespace tinyngine