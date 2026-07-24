#pragma once

#include "Object3D.h"

namespace Kizuna {
    /// <summary>
    /// 三角平面オブジェクト
    /// </summary>
    class TriangleObject : public Object3D {
    public:
        explicit TriangleObject(
            const Vector3 &left, const Vector3 &top, const Vector3 &right)
            : left_(left), top_(top), right_(right) {}

        /// <summary>
        /// 平面三角形を生成する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="color">描画色</param>
        void Create(ID3D12Device *device, uint32_t color, bool enableLighting) override;

        void CreatePlaneTriangle(
            ID3D12Device *device,
            Vector3 left, Vector3 top, Vector3 right,
            uint32_t color, bool enableLighting
        );

    private:
        /// 左頂点
        Vector3 left_;
      
        /// 上頂点
        Vector3 top_;
      
        /// 右頂点
        Vector3 right_;
    };
}