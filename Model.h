#pragma once

#include <string>

#include "ModelLoader.h"

#include "VertexBuffer.h"
#include "ConstantBuffer.h"

#include "Material.h"
#include "Texture.h"
#include "Render.h"

#include "Transform.h"
#include "Camera.h"

class Model {
public:
	void Initialize(
		ID3D12Device *device,
		const std::string &directoryPath,
		const std::string &fileName,
		uint32_t color, bool enableLighting
	);

	void Update(Camera *camera);

	void Draw(ID3D12GraphicsCommandList *commandList, Texture &texture);

	// getter
	ModelData &GetModelData() { return modelData_; }
	Transform &GetTransform() { return transform_; }
	Material &GetMaterial() { return material_; }

private:
	Render render_;

	ModelData modelData_;

	VertexBuffer vertexBuffer_;

	ConstantBuffer transformationMatrixBuffer_;
	TransformationMatrix *transformationMatrixData_ = nullptr;

	Material material_;

	Transform transform_{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
};