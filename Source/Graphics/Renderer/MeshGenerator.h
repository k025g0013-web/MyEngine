#pragma once

#include <cstdint>
#include <vector>

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"

namespace Kizuna {

    /// <summary>
    /// 基本形状のメッシュデータを生成するクラス
    /// </summary>
    /// <remarks>
    /// GPUリソースの生成は行わず、
    /// 頂点データおよびインデックスデータの生成のみを担当する。
    /// </remarks>
    class MeshGenerator {
    public:

        /// <summary>
         /// 矩形スプライトの頂点データ・インデックスデータを生成する
         /// </summary>
         /// <param name="vertices">生成先の頂点データ</param>
         /// <param name="indices">生成先のインデックスデータ</param>
         /// <param name="left">左端座標</param>
         /// <param name="top">上端座標</param>
         /// <param name="right">右端座標</param>
         /// <param name="bottom">下端座標</param>
        static void CreateSprite(
            std::vector<VertexData> &vertices,
            std::vector<uint32_t> &indices,
            float left,
            float top,
            float right,
            float bottom
        );

        /// <summary>
        /// 三角形メッシュの頂点データを生成する
        /// </summary>
        static void CreateTriangle(
            std::vector<VertexData> &vertices,
            Vector3 left,
            Vector3 top,
            Vector3 right
        );

        /// <summary>
        /// 四角形メッシュの頂点データを生成する
        /// </summary>
        static void CreatePlane(
            std::vector<VertexData> &vertices,
            Vector3 center,
            float width,
            float depth
        );

        /// <summary>
        /// 球体メッシュの頂点データとインデックスデータを生成する
        /// </summary>
        static void CreateSphere(
            std::vector<VertexData> &vertices,
            std::vector<uint32_t> &indices,
            uint32_t subdivision
        );
    };
}