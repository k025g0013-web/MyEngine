#include "Render.h"

#include "MathFunctions.h"

void Render::CreateVertexBuffer(
    ID3D12Device *device,
    VertexBuffer &vertexBuffer,
    const std::vector<VertexData> &vertices
) {
    // 頂点リソースを作る
    vertexBuffer.Initialize(device, sizeof(VertexData) * vertices.size(), sizeof(VertexData));

    // 書き込むためのアドレスを取得
    VertexData *vertexData = static_cast<VertexData *>(vertexBuffer.Map());

    // 頂点データをリリースにコピー
    std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
}

void Render::CreateIndexBuffer(
    ID3D12Device *device,
    IndexBuffer &indexBuffer,
    const std::vector<uint32_t> &indices
) {
    // 頂点インデックス
    indexBuffer.Initialize(device, sizeof(uint32_t) * indices.size());

    // 書き込むためのアドレスを取得
    uint32_t *indexData = static_cast<uint32_t *>(indexBuffer.Map());

    // 頂点データをリリースにコピー
    std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
}

void Render::CreateTransformationMatrixBuffer(
    ID3D12Device *device,
    ConstantBuffer &constantBuffer,
    TransformationMatrix *&data
) {
    // WVP用のリソースを作る
    constantBuffer.Initialize(device, sizeof(TransformationMatrix));

    // データを書き込む
    data = static_cast<TransformationMatrix *>(constantBuffer.Map());

    // 単位行列を書き込んでおく
    data->WVP = MakeIdentity4x4();
    data->World = MakeIdentity4x4();
}

void Render::CreateSpriteVertices(std::vector<VertexData> &vertices, float left, float top, float right, float bottom) {
    vertices = {
    { {  left, bottom, 0.0f, 1.0f}, {0.0f, 1.0f}, {0,0,-1} },   // 左下
    { {  left,    top, 0.0f, 1.0f}, {0.0f, 0.0f}, {0,0,-1} },   // 左上
    { { right, bottom, 0.0f, 1.0f}, {1.0f, 1.0f}, {0,0,-1} },   // 右下
    { { right,    top, 0.0f, 1.0f}, {1.0f, 0.0f}, {0,0,-1} },   // 右上
    };
}

void Render::CreateSpriteIndices(std::vector<uint32_t> &indices) {
    indices = {
        0, 1, 2,
        1, 3, 2
    };
}

void Render::CreateTriangleVertices(
    std::vector<VertexData> &vertices,
    Vector3 left, Vector3 top, Vector3 right
) {
    vertices = {
    { {  left.x,  left.y,  left.z, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },    // 左
    { {   top.x,   top.y,   top.z, 1.0f }, { 0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f } },    // 右
    { { right.x, right.y, right.z, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },    // 上
    };
}