#pragma once

#include <d3d12.h>

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

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
    MaterialData *GetMaterialData() const { return materialData_; }

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