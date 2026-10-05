#include "TriangleObject.h"

namespace Kizuna {

    void TriangleObject::Create(
        ID3D12Device *device, ID3D12GraphicsCommandList *commandList,
        uint32_t color, bool enableLighting) {
        // commandListはプリミティブでは使用しない
        (void)commandList;

        // 頂点データを生成する
        std::vector<VertexData> vertices;

        meshGenerator_.CreateTriangle(
            vertices, left_, top_, right_);

        // メッシュを生成する
        mesh_.Create(device, vertices);

        // 共通初期化
        InitializeMaterial(device, color, enableLighting);
    }
}