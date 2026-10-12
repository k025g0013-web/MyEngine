#pragma once

#pragma comment(lib, "d3d12.lib")

#include <wrl.h>
#include <d3d12.h>

namespace Kizuna {

    class Logger;

    /// <summary>
    /// DirectX12のRootSignatureを管理するクラス
    /// </summary>
    class RootSignature {
    public:

        /// <summary>
        /// RootSignatureの種類
        /// </summary>
        enum class Type {
            /// ライト計算を使用する3D描画用
            Skinny3D,

            /// ライト計算を使用しない2D描画用
            Skinny2D,

            /// 壁越し描画用3D
            ThroughWall3D,

            /// パーティクル描画用
            Particles,
        };

    public:

        /// <summary>
        /// 指定された種類のRootSignatureを生成する
        /// </summary>
        void Create(
            ID3D12Device *device,
            Logger *logger,
            Type type
        );

        /// <summary>
        /// 生成済みRootSignatureを取得する
        /// </summary>
        ID3D12RootSignature *GetRootSignature() const {
            return rootSignature_.Get();
        }

    private:

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    };
}