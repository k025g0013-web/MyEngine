#pragma once

#pragma comment(lib, "d3d12.lib")

#include <wrl.h>
#include <d3d12.h>

class Logger;

/// <summary>
/// DirectX12のRootSignatureを管理するクラス
/// </summary>
/// <remarks>
/// シェーダーで使用するCBVやSRVなどのリソース配置を定義し、
/// 描画用途に応じたRootSignatureを生成する。
/// </remarks>
class RootSignature {
public:

    /// <summary>
    /// RootSignatureの用途を定義する列挙型
    /// </summary>
    enum class Type {
        /// ライト計算を使用する3D描画用
        Skinny3D,

        /// ライト計算を使用しない2D描画用
        Skinny2D,
    };

public:

    /// <summary>
    /// 標準的な3D描画用RootSignatureを生成する
    /// </summary>
    /// <remarks>
    /// Transform、Material、Texture、Lightingなど、
    /// 3Dオブジェクト描画に必要なリソースを設定する。
    /// </remarks>
    /// <param name="device">DirectX12デバイス</param>
    /// <param name="logger">ログ出力クラス</param>
    void CreateSkinny3D(ID3D12Device *device, Logger *logger);


    /// <summary>
    /// 2D描画用RootSignatureを生成する
    /// </summary>
    /// <remarks>
    /// ライティングを使用しない2D描画向けの
    /// リソース配置を設定する。
    /// </remarks>
    /// <param name="device">DirectX12デバイス</param>
    /// <param name="logger">ログ出力クラス</param>
    void CreateSkinny2D(ID3D12Device *device, Logger *logger);


    /// <summary>
    /// 壁越し描画用3D RootSignatureを生成する
    /// </summary>
    /// <remarks>
    /// 通常の3D描画用リソースに加えて、
    /// 壁越し描画専用の定数バッファを設定する。
    /// </remarks>
    /// <param name="device">DirectX12デバイス</param>
    /// <param name="logger">ログ出力クラス</param>
    void CreateThroughWall3D(ID3D12Device *device, Logger *logger);


    /// <summary>
    /// 生成済みRootSignatureを取得する
    /// </summary>
    /// <returns>DirectX12 RootSignature</returns>
    ID3D12RootSignature *GetRootSignature() const {
        return rootSignature_.Get();
    }

private:

    /// 生成されたRootSignature
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};