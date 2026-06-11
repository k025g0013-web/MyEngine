#pragma once

#include <vector>

#include "VertexBuffer.h"
#include "ConstantBuffer.h"

#include "Material.h"
#include "Texture.h"
#include "Render.h"

#include "Transform.h"
#include "Camera.h"

class Sprite {
public:
	void Initialize(
		ID3D12Device *device, 
		float left, float top, float right, float bottom, 
		uint32_t color
	);

	void Update(uint32_t windowWidth, uint32_t windowHeight);

	void Draw(ID3D12GraphicsCommandList *commandList, Texture &texture);

	// getter
	Transform &GetTransform() { return transform_; }
	Transform &GetUVTransform() { return uvTransform_; }
	Material &GetMaterial() { return material_; }

private:
	Render render_;

	std::vector<VertexData> vertices_;
	std::vector<uint32_t> indices_;

	VertexBuffer vertexBuffer_;
	IndexBuffer indexBuffer_;

	ConstantBuffer transformationMatrixBuffer_;
	TransformationMatrix *transformationMatrixData_ = nullptr;

	Material material_;

	Transform transform_{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	
	Transform uvTransform_{ 
		{ 1.0f, 1.0f, 1.0f }, 
		{ 0.0f, 0.0f, 0.0f }, 
		{ 0.0f, 0.0f, 0.0f } 
	};
};
