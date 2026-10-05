#pragma once

#include "Object/Common/Object3D.h"

namespace Kizuna {

    /// <summary>
    /// 三角平面オブジェクト
    /// </summary>
    class TriangleObject : public Object3D {
    public:
        explicit TriangleObject(
            const Vector3 &left,
            const Vector3 &top,
            const Vector3 &right)
            : left_(left),
            top_(top),
            right_(right) {}

        /// <summary>
        /// 三角形を生成する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="commandList">コマンドリスト</param>
        /// <param name="color">描画色</param>
        /// <param name="enableLighting">ライティングを有効にするか</param>
        void Create(
            ID3D12Device *device,
            ID3D12GraphicsCommandList *commandList,
            uint32_t color,
            bool enableLighting) override;

    private:
        // 左頂点
        Vector3 left_;

        // 上頂点
        Vector3 top_;

        // 右頂点
        Vector3 right_;
    };
}