#pragma once

#include <string>
#include <vector>

#include "MeshBuffer.h"
#include "Math/Vector.h"

struct MaterialSource {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialSource material;
};

class ModelLoader {
public:
	static MaterialSource LoadMaterialTemplateFile(
		const std::string &directoryPath,
		const std::string &fileName
	);

	static ModelData LoadObjFile(
		const std::string &directoryPath,
		const std::string &fileName
	);
};