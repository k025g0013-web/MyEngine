#pragma once

#include <vector>

#include "Graphics/Resource/Mesh.h"
#include "Graphics/Resource/MeshBuffer.h"

namespace Kizuna {

    /// <summary>
    /// モデルデータを保持する構造体
    /// </summary>
    struct ModelData {
        // モデルの頂点データ
        std::vector<VertexData> vertices;

        // GPUに生成したメッシュ
        Mesh mesh;
    };
}