#pragma once

#include <d3d12.h>

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

namespace Kizuna {

    /// <summary>
    /// マテリアル情報をGPUへ転送するための構造体
    /// </summary>
    /// <remarks>
    /// 色・ライティング設定・UV変換行列を保持し、
    /// ピクセルシェーダで使用される定数バッファとして利用する。
    /// </remarks>
    struct MaterialData {

        /// マテリアルカラー
        Vector4 color;

        /// ライティングを有効にするか
        int32_t enableLighting;

        /// ConstantBufferのアライメント調整用
        float padding[3];

        /// UV座標変換行列
        Matrix4x4 uvTransform;
    };

    /// <summary>
    /// 壁越し描画専用マテリアル情報
    /// </summary>
    /// <remarks>
    /// 色・描画スタイル・UV変換行列を保持し、
    /// ピクセルシェーダで使用される定数バッファとして利用する。
    /// </remarks>
    struct ThroughWallMaterialData {

        /// 描画色
        Vector4 color;

        /// 描画スタイル
        int32_t style;

        /// ConstantBufferのアライメント調整用
        float padding[3];

        /// UV座標変換
        Matrix4x4 uvTransform;
    };

    /// <summary>
    /// マテリアル定数バッファを管理するクラス
    /// </summary>
    /// <remarks>
    /// オブジェクトごとの色やライティング設定、UV変換を管理し、
    /// GPUへ定数バッファとして転送する。
    /// </remarks>
    class Material {
    public:

        /// <summary>
        /// マテリアルを初期化する
        /// </summary>
        /// <param name="device">Direct3Dデバイス</param>
        /// <param name="color">初期色（0xRRGGBBAA形式）</param>
        /// <param name="enableLighting">ライティングを有効にするか</param>
        void Initialize(
            ID3D12Device *device,
            uint32_t color,
            bool enableLighting
        );

        /// <summary>
        /// マテリアルデータを取得する
        /// </summary>
        MaterialData *GetMaterialData() const {
            return materialData_;
        }

        /// <summary>
        /// 定数バッファのGPU仮想アドレスを取得する
        /// </summary>
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
            return constantBuffer_.GetGPUVirtualAddress();
        }

    private:

        /// マテリアル用定数バッファ
        MeshBuffer constantBuffer_;

        /// CPU側から更新するマテリアルデータ
        MaterialData *materialData_ = nullptr;
    };


    /// <summary>
    /// 壁越し描画用マテリアル情報を管理するクラス
    /// </summary>
    /// <remarks>
    /// 壁越し描画時に使用する色や描画スタイル、
    /// UV変換を管理し、GPUへ定数バッファとして転送する。
    /// </remarks>
    class ThroughWallMaterial {
    public:

        /// <summary>
        /// 壁越し描画用マテリアルを初期化する
        /// </summary>
        /// <param name="device">Direct3Dデバイス</param>
        /// <param name="color">初期色（0xRRGGBBAA形式）</param>
        void Initialize(
            ID3D12Device *device,
            uint32_t color
        );

        /// <summary>
        /// 壁越し描画用マテリアルデータを取得する
        /// </summary>
        ThroughWallMaterialData *GetMaterialData() const {
            return materialData_;
        }

        /// <summary>
        /// 定数バッファのGPU仮想アドレスを取得する
        /// </summary>
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
            return constantBuffer_.GetGPUVirtualAddress();
        }

    private:

        /// 壁越し描画用マテリアル定数バッファ
        MeshBuffer constantBuffer_;

        /// CPU側から更新する壁越し描画用マテリアルデータ
        ThroughWallMaterialData *materialData_ = nullptr;
    };
}