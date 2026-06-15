#include "Mesh.h"

#include "Renderer/Renderer.h"

void Mesh::Create(
    ID3D12Device *device, const std::vector<VertexData> &vertices
) {
    vertexCount_ = static_cast<uint32_t>(vertices.size());

    Renderer render;
    render.CreateVertexBuffer(device, vertexBuffer_, vertices);

    hasIndexBuffer_ = false;
}

void Mesh::Create(
    ID3D12Device *device, const std::vector<VertexData> &vertices, const std::vector<uint32_t> &indices
) {
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    indexCount_ = static_cast<uint32_t>(indices.size());

    Renderer render;
    render.CreateVertexBuffer(device, vertexBuffer_, vertices);
    render.CreateIndexBuffer(device, indexBuffer_, indices);

    hasIndexBuffer_ = true;
}

void Mesh::Bind(ID3D12GraphicsCommandList *commandList) {
    commandList->IASetVertexBuffers(0, 1, vertexBuffer_.GetVertexBufferView());

    if (hasIndexBuffer_) {
        commandList->IASetIndexBuffer(indexBuffer_.GetIndexBufferView());
        
        // 描画!(DrawCall/ドローコール)
        commandList->DrawIndexedInstanced(indexCount_, 1, 0, 0, 0);

    } else {
        // 描画!(DrawCall/ドローコール)
        commandList->DrawInstanced(vertexCount_, 1, 0, 0);
    }
}
