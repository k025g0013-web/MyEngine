#include "Renderer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Math/FunctionMatrix.h"

void Renderer::CreateVertexBuffer(
    ID3D12Device *device,
    MeshBuffer &vertexBuffer,
    const std::vector<VertexData> &vertices
) {
    // GPU上に頂点バッファを生成する
    vertexBuffer.InitializeAsVertex(device, sizeof(VertexData) * vertices.size(), sizeof(VertexData));

    // CPUから書き込むためにバッファをマッピングする
    VertexData *vertexData = static_cast<VertexData *>(vertexBuffer.Map());

    // CPU側で生成した頂点データをGPUリソースへコピーする
    std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
}

void Renderer::CreateIndexBuffer(
    ID3D12Device *device,
    MeshBuffer &indexBuffer,
    const std::vector<uint32_t> &indices
) {
    // GPU上にインデックスバッファを生成する
    indexBuffer.InitializeAsIndex(device, sizeof(uint32_t) * indices.size());

    // CPUから書き込むためにバッファをマッピングする
    uint32_t *indexData = static_cast<uint32_t *>(indexBuffer.Map());

    // CPU側で生成したインデックスデータをGPUリソースへコピーする
    std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
}

void Renderer::CreateTransformationMatrixBuffer(
    ID3D12Device *device,
    MeshBuffer &constantBuffer,
    TransformationMatrix *&data
) {
    // ワールド行列・WVP行列を格納する定数バッファを生成する
    constantBuffer.InitializeAsConstant(device, sizeof(TransformationMatrix));

    // CPUから更新できるようにバッファをマッピングする
    data = static_cast<TransformationMatrix *>(constantBuffer.Map());

    // 初期状態では変換を行わないよう単位行列で初期化する
    data->WVP = Math::MakeIdentity<Matrix4x4>();
    data->World = Math::MakeIdentity<Matrix4x4>();
}

void Renderer::CreateSprite(
    std::vector<VertexData> &vertices,
    std::vector<uint32_t> &indices,
    float left,
    float top,
    float right,
    float bottom
) {
    // 2枚の三角形で構成される矩形メッシュを生成する
    vertices = {
        {{ left, bottom, 0.0f, 1.0f }, {0.0f,1.0f}, {0,0,-1}},
        {{ left, top,    0.0f, 1.0f }, {0.0f,0.0f}, {0,0,-1}},
        {{ right,bottom, 0.0f, 1.0f }, {1.0f,1.0f}, {0,0,-1}},
        {{ right,top,    0.0f, 1.0f }, {1.0f,0.0f}, {0,0,-1}},
    };

    indices = {
        0,1,2,
        1,3,2
    };
}

void Renderer::CreateTriangle(
    std::vector<VertexData> &vertices,
    Vector3 left,
    Vector3 top,
    Vector3 right
) {
    // 単一の三角形メッシュを生成する
    vertices = {
        {{left.x,left.y,left.z,1.0f},{0.0f,1.0f},{0.0f,0.0f,-1.0f}},
        {{top.x,top.y,top.z,1.0f},{0.5f,0.0f},{0.0f,0.0f,-1.0f}},
        {{right.x,right.y,right.z,1.0f},{1.0f,1.0f},{0.0f,0.0f,-1.0f}},
    };
}

void Renderer::CreateSphere(
    std::vector<VertexData> &vertices,
    std::vector<uint32_t> &indices,
    uint32_t subdivision
) {
    // 分割数から必要な頂点数を算出する
    const uint32_t vertexCount = (subdivision + 1) * (subdivision + 1);

    // 1マスを2枚の三角形で構成するため、1マスあたり6インデックス必要
    const uint32_t indexCount = subdivision * subdivision * 6;

    vertices.resize(vertexCount);
    indices.resize(indexCount);

    // 経度方向1区画あたりの角度
    const float kLonEvery = static_cast<float>(M_PI) * 2.0f / static_cast<float>(subdivision);

    // 緯度方向1区画あたりの角度
    const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(subdivision);

    // 緯度方向に頂点を生成する（-90°～90°）
    for (uint32_t latIndex = 0; latIndex <= subdivision; ++latIndex) {

        float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;

        // 経度方向に頂点を生成する（0°～360°）
        for (uint32_t lonIndex = 0; lonIndex <= subdivision; ++lonIndex) {

            float lon = lonIndex * kLonEvery;

            uint32_t index = latIndex * (subdivision + 1) + lonIndex;

            // 緯度・経度から球面上の座標を計算する
            Vector3 pos = {
                std::cos(lat) * std::cos(lon),
                std::sin(lat),
                std::cos(lat) * std::sin(lon),
            };

            // 球全体へテクスチャを貼り付けるためのUV座標を計算する
            float u = float(lonIndex) / subdivision;
            float v = 1.0f - float(latIndex) / subdivision;

            // 法線は球の中心から頂点へ向かうベクトルを使用する
            vertices[index] = {
                .position{pos.x,pos.y,pos.z,1.0f},
                .texcoord{u,v},
                .normal{pos.x,pos.y,pos.z},
            };

            if (latIndex < subdivision && lonIndex < subdivision) {

                size_t start =
                    (static_cast<size_t>(latIndex) * subdivision + lonIndex) * 6;

                // 隣接する4頂点を取得する
                uint32_t a = index;
                uint32_t b = (latIndex + 1) * (subdivision + 1) + lonIndex;
                uint32_t c = latIndex * (subdivision + 1) + (lonIndex + 1);
                uint32_t d = (latIndex + 1) * (subdivision + 1) + (lonIndex + 1);

                // 四角形を2枚の三角形へ分割する
                indices[start + 0] = a;
                indices[start + 1] = b;
                indices[start + 2] = c;

                indices[start + 3] = c;
                indices[start + 4] = b;
                indices[start + 5] = d;
            }
        }
    }
}