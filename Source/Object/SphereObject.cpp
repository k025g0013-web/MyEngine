#include "SphereObject.h"

namespace Kizuna {
    void SphereObject::Create(
        ID3D12Device *device, uint32_t color, bool enableLighting) {
        // 球体の頂点・インデックス生成
        renderer_.CreateSphere(
            vertices_, indices_, subdivision_);

        // メッシュ生成
        mesh_.Create(device, vertices_, indices_);

        // 共通初期化
        InitializeMaterial(device, color, enableLighting);
    }
}