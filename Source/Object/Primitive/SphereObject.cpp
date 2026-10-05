#include "SphereObject.h"

namespace Kizuna {
    void SphereObject::Create(
        ID3D12Device *device, ID3D12GraphicsCommandList *commandList,
        uint32_t color, bool enableLighting) {
        // commandListはプリミティブでは使用しない
        (void)commandList;

        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;

        meshGenerator_.CreateSphere(
            vertices,
            indices,
            subdivision_);

        // メッシュ生成
        mesh_.Create(device, vertices, indices);

        // 共通初期化
        InitializeMaterial(device, color, enableLighting);
    }
}