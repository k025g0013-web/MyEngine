#pragma once

#include "Object3D.h"

namespace Kizuna {
    /// <summary>
    /// 平面四角形オブジェクト
    /// </summary>
    class PlaneObject : public Object3D {
    public:
        explicit PlaneObject(
            const Vector3 center, const float width, const float depth)
            : center_(center), width_(width), depth_(depth){}

        /// <summary>
        /// 平面四角形を生成する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="color">描画色</param>
        void Create(ID3D12Device *device, uint32_t color, bool enableLighting) override;

    private:
        /// 中央座標
        Vector3 center_;
        
        /// 幅
        float width_;
        
        /// 深さ
        float depth_;
    };
}