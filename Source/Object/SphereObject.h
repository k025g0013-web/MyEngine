#pragma once

#include "Object3D.h"

namespace Kizuna {

    /// <summary>
    /// 球体オブジェクト
    /// </summary>
    class SphereObject : public Object3D {
    public:
        explicit SphereObject(uint32_t subdivision = 16)
            : subdivision_(subdivision) {}

        /// <summary>
        /// 球体を生成する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="commandList">コマンドリスト</param>
        /// <param name="color">描画色</param>
        /// <param name="enableLighting">ライティングを有効にするか</param>
        void Create(
            ID3D12Device *device ,ID3D12GraphicsCommandList *commandList,
            uint32_t color, bool enableLighting) override;
        // commandListはプリミティブでは使用しない

    private:
        // 分割数
        uint32_t subdivision_;
    };
}