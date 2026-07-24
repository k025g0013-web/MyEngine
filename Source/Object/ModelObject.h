#pragma once

#include "Object3D.h"

namespace Kizuna {
    /// <summary>
    /// モデルオブジェクト
    /// </summary>
    class ModelObject : public Object3D {
    public:
        explicit ModelObject(std::string fileName)
            : fileName_(std::move(fileName)) {}
        
        /// <summary>
        /// モデルを生成する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="color">描画色</param>
        void Create(ID3D12Device *device, uint32_t color, bool enableLighting) override;

    private:
        // モデルファイル名
        std::string fileName_;
    };
}