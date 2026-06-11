#pragma once

#include <string>
#include <vector>

#include "VertexData.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

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