#include "Renderer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Math/FunctionMatrix.h"

void Renderer::CreateVertexBuffer(
    ID3D12Device *device,
    MeshBuffer &vertexBuffer,
    const std::vector<VertexData> &vertices
) {
    // 頂点リソースを作る
    vertexBuffer.InitializeAsVertex(device, sizeof(VertexData) * vertices.size(), sizeof(VertexData));

    // 書き込むためのアドレスを取得
    VertexData *vertexData = static_cast<VertexData *>(vertexBuffer.Map());

    // 頂点データをリリースにコピー
    std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
}

void Renderer::CreateIndexBuffer(
    ID3D12Device *device,
    MeshBuffer &indexBuffer,
    const std::vector<uint32_t> &indices
) {
    // 頂点インデックス
    indexBuffer.InitializeAsIndex(device, sizeof(uint32_t) * indices.size());

    // 書き込むためのアドレスを取得
    uint32_t *indexData = static_cast<uint32_t *>(indexBuffer.Map());

    // 頂点データをリリースにコピー
    std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
}

void Renderer::CreateTransformationMatrixBuffer(
    ID3D12Device *device,
    MeshBuffer &constantBuffer,
    TransformationMatrix *&data
) {
    // WVP用のリソースを作る
    constantBuffer.InitializeAsConstant(device, sizeof(TransformationMatrix));

    // データを書き込む
    data = static_cast<TransformationMatrix *>(constantBuffer.Map());

    // 単位行列を書き込んでおく
    data->WVP = Math::MakeIdentity<Matrix4x4>();
    data->World = Math::MakeIdentity<Matrix4x4>();
}

void Renderer::CreateSprite(std::vector<VertexData> &vertices, std::vector<uint32_t> &indices, float left, float top, float right, float bottom) {
    vertices = {
    { {  left, bottom, 0.0f, 1.0f}, {0.0f, 1.0f}, {0,0,-1} },   // 左下
    { {  left,    top, 0.0f, 1.0f}, {0.0f, 0.0f}, {0,0,-1} },   // 左上
    { { right, bottom, 0.0f, 1.0f}, {1.0f, 1.0f}, {0,0,-1} },   // 右下
    { { right,    top, 0.0f, 1.0f}, {1.0f, 0.0f}, {0,0,-1} },   // 右上
    };

    indices = {
       0, 1, 2,
       1, 3, 2
    };
}

void Renderer::CreateTriangle(
    std::vector<VertexData> &vertices,
    Vector3 left, Vector3 top, Vector3 right
) {
    vertices = {
    { {  left.x,  left.y,  left.z, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },    // 左
    { {   top.x,   top.y,   top.z, 1.0f }, { 0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f } },    // 右
    { { right.x, right.y, right.z, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },    // 上
    };
}

void Renderer::CreateSphere(
    std::vector<VertexData> &vertices,
    std::vector<uint32_t> &indices,
    uint32_t subdivision
) {
    const uint32_t vertexCount = (subdivision + 1) * (subdivision + 1);

    const uint32_t indexCount = subdivision * subdivision * 6;

    vertices.resize(vertexCount);
    indices.resize(indexCount);

    const float kLonEvery = static_cast<float>(M_PI) * 2.0f / static_cast<float>(subdivision);

    const float kLatEvery =
        static_cast<float>(M_PI) / static_cast<float>(subdivision);

    // 緯度の方向に分割 -pi/2 ~ pi/2
    for (uint32_t latIndex = 0; latIndex <= subdivision; ++latIndex) {
        float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;

        // 経度の方向に分割 0 ~ 2pi
        for (uint32_t lonIndex = 0; lonIndex <= subdivision; ++lonIndex) {
            float lon = lonIndex * kLonEvery;

            // 頂点生成
            uint32_t index = latIndex * (subdivision + 1) + lonIndex;

            // world座標を求める
            Vector3 pos = {
                std::cos(lat) * std::cos(lon),
                std::sin(lat),
                std::cos(lat) * std::sin(lon),
            };

            // Texcoordを計算する
            float u = float(lonIndex) / subdivision;
            float v = 1.0f - float(latIndex) / subdivision;

            // 頂点データの作成
            vertices[index] = {
                .position{pos.x, pos.y, pos.z, 1.0f},
                .texcoord{u, v},
                .normal{pos.x, pos.y, pos.z},
            };

            // インデックスデータの生成
            if (latIndex < subdivision && lonIndex < subdivision) {
                size_t start = (static_cast<size_t>(latIndex) * subdivision + lonIndex) * 6;

                // 四角形の4頂点
                uint32_t a = index;
                uint32_t b = (latIndex + 1) * (subdivision + 1) + lonIndex;
                uint32_t c = latIndex * (subdivision + 1) + (lonIndex + 1);
                uint32_t d = (latIndex + 1) * (subdivision + 1) + (lonIndex + 1);

                // インデックスリソースにデータを書き込む
                indices[start + 3] = c;	 indices[start + 4] = b;	 indices[start + 5] = d;
                indices[start + 0] = a;	 indices[start + 1] = b;	 indices[start + 2] = c;
            }
        }
    }
}