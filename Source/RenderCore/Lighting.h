#pragma once

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"

/// <summary>
/// 平行光源の情報を保持する構造体
/// </summary>
/// <remarks>
/// 色・方向・光の強さ・ライティング方式をGPUへ転送する。
/// </remarks>
struct DirectionalLight {

    /// 光の色
    Vector4 color;

    /// 光の向き
    Vector3 direction;

    /// 光の強さ
    float intensity;

    /// ライティング方式
    int lightType;

    /// ConstantBufferのアライメント調整用
    float padding[3];
};

/// <summary>
/// シーン全体で使用する平行光源を管理するクラス
/// </summary>
/// <remarks>
/// ライト情報を更新し、GPUへ定数バッファとして転送する。
/// ライティング方式の切り替えも担当する。
/// </remarks>
class Lighting {
public:

    /// <summary>
    /// ライティング方式
    /// </summary>
    enum class LightingType {
        None,
        Lambert,
        Half_Lambert,
    };

public:

    /// <summary>
    /// ライトを初期化する
    /// </summary>
    /// <param name="device">Direct3Dデバイス</param>
    void Initialize(ID3D12Device *device);

    /// <summary>
    /// ライト情報を更新する
    /// </summary>
    /// <remarks>
    /// 光の方向ベクトルを正規化し、
    /// ライティング計算で正しい結果になるよう補正する。
    /// </remarks>
    void Update();

    /// <summary>
    /// ライト情報をGPUへ設定する
    /// </summary>
    /// <param name="rootParameterIndex">RootParameter番号</param>
    /// <param name="commandList">描画コマンドリスト</param>
    void Bind(
        UINT rootParameterIndex,
        ID3D12GraphicsCommandList *commandList
    );

    /// <summary>
    /// ライティング方式を変更する
    /// </summary>
    /// <param name="type">設定するライティング方式</param>
    void SetLightType(LightingType type);

    /// <summary>
    /// ライト情報を取得する
    /// </summary>
    DirectionalLight *GetLightingData() { return lightingData_; }

    /// <summary>
    /// 現在のライティング方式を取得する
    /// </summary>
    LightingType GetLightType() const { return currentLightType_; }

private:

    /// ライト用定数バッファ
    MeshBuffer constantBuffer_;

    /// CPU側から更新するライト情報
    DirectionalLight *lightingData_ = nullptr;

    /// 現在のライティング方式
    LightingType currentLightType_{};
};