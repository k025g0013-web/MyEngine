#pragma once

#include <vector>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

#include "VertexData.h"
#include "TransformationMatrix.h"

class Render {
public:
    // Model
    void CreateVertexBuffer(
        ID3D12Device *device,
        VertexBuffer &vertexBuffer,
        const std::vector<VertexData> &vertices
    );

    void CreateIndexBuffer(
        ID3D12Device *device,
        IndexBuffer &indexBuffer,
        const std::vector<uint32_t> &indices
    );

    void CreateTransformationMatrixBuffer(
        ID3D12Device *device,
        ConstantBuffer &constantBuffer,
        TransformationMatrix *&data
    );

    // Sprite
    void CreateSpriteVertices(
        std::vector<VertexData> &vertices,
        float left,
        float top,
        float right,
        float bottom
    );

    void CreateSpriteIndices(
        std::vector<uint32_t> &indices
    );

    // Triangle
    void CreateTriangleVertices(
        std::vector<VertexData> &vertices,
        Vector3 left, Vector3 top, Vector3 right
    );
};