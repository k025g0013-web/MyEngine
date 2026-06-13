#pragma once

#include <vector>

#include "MeshBuffer.h"
#include "TransformationMatrix.h"

class Renderer {
public:
    // Model
    void CreateVertexBuffer(
        ID3D12Device *device,
        MeshBuffer &vertexBuffer,
        const std::vector<VertexData> &vertices
    );

    void CreateIndexBuffer(
        ID3D12Device *device,
        MeshBuffer &indexBuffer,
        const std::vector<uint32_t> &indices
    );

    void CreateTransformationMatrixBuffer(
        ID3D12Device *device,
        MeshBuffer &constantBuffer,
        TransformationMatrix *&data
    );

    // Sprite
    void CreateSprite(
        std::vector<VertexData> &vertices,
        std::vector<uint32_t> &indices,
        float left,
        float top,
        float right,
        float bottom
    );

    // Triangle
    void CreateTriangle(
        std::vector<VertexData> &vertices,
        Vector3 left, Vector3 top, Vector3 right
    );

    // Sphere
    void CreateSphere(
        std::vector<VertexData> &vertices,
        std::vector<uint32_t> &indices,
        uint32_t subdivision
    );
};