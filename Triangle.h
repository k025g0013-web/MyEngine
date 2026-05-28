#pragma once

#include <vector>

#include "VertexBuffer.h"
#include "ConstantBuffer.h"

#include "Material.h"
#include "Texture.h"
#include "Render.h"

#include "Transform.h"
#include "Camera.h"

class Triangle {
public:
    void Initialize(
        ID3D12Device *device,
        Vector3 left, Vector3 top, Vector3 right,
        uint32_t color
    );

    void Update(Camera *camera);

    void Draw(ID3D12GraphicsCommandList *commandList, Texture &texture);

    // getter
    Transform &GetTransform() { return transform_; }
    Material &GetMaterial() { return material_; }

private:
    Render render_;

    std::vector<VertexData> vertices_;

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