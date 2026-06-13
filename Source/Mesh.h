#pragma once

#include <vector>

#include "MeshBuffer.h"

class Mesh {
public:
    void Create(
        ID3D12Device *device, const std::vector<VertexData> &vertices
    );

    void Create(
        ID3D12Device *device, const std::vector<VertexData> &vertices, const std::vector<uint32_t> &indices
    );

    void Bind(ID3D12GraphicsCommandList *commandList);

    uint32_t GetVertexCount() const {return vertexCount_;}
    uint32_t GetIndexCount() const {return indexCount_;}

    bool HasIndexBuffer() const {return hasIndexBuffer_;}

private:
    MeshBuffer vertexBuffer_;
    MeshBuffer indexBuffer_;

    uint32_t vertexCount_ = 0;
    uint32_t indexCount_ = 0;

    bool hasIndexBuffer_ = false;
};