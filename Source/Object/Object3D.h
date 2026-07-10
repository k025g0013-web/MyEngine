#pragma once

#include <string>

#include "Asset/ModelLoader.h"

#include "Graphics/Resource/MeshBuffer.h"
#include "Graphics/Resource/Texture.h"
#include "RenderCore/Camera.h"
#include "RenderCore/Material.h"
#include "RenderCore/Mesh.h"
#include "Renderer/Renderer.h"

/// <summary>
/// 描画方法を切り替えるためのレンダーレイヤ
/// </summary>
enum class RenderLayer {
    kDefault,      // 通常描画
    kThroughWall   // 壁越し描画対象
};

/// <summary>
/// 3Dオブジェクトを生成・更新・描画するクラス
/// </summary>
/// <remarks>
/// プリミティブやOBJモデルの生成、マテリアル管理、
/// ワールド行列・WVP行列の更新、および描画処理を担当する。
/// 壁越し描画用マテリアルも保持しており、描画方法を切り替えられる。
/// </remarks>
class Object3D {
public:
    //=====================================================================
    // オブジェクト生成
    //=====================================================================

    /// <summary>
    /// 平面三角形を生成する
    /// </summary>
    /// <param name="device">DirectXデバイス</param>
    /// <param name="left">左頂点</param>
    /// <param name="top">上頂点</param>
    /// <param name="right">右頂点</param>
    /// <param name="color">描画色</param>
    /// <param name="enableLighting">ライティングを有効にするか</param>
    void CreatePlaneTriangle(
        ID3D12Device *device,
        Vector3 left, Vector3 top, Vector3 right,
        uint32_t color, bool enableLighting
    );

    /// <summary>
    /// 球体メッシュを生成する
    /// </summary>
    /// <param name="device">DirectXデバイス</param>
    /// <param name="subdivision">球の分割数</param>
    /// <param name="color">描画色</param>
    /// <param name="enableLighting">ライティングを有効にするか</param>
    void CreateSphere(
        ID3D12Device *device,
        uint32_t subdivision,
        uint32_t color, bool enableLighting
    );

    /// <summary>
    /// OBJモデルを読み込み生成する
    /// </summary>
    /// <param name="device">DirectXデバイス</param>
    /// <param name="fileName">読み込むモデル名</param>
    /// <param name="color">描画色</param>
    /// <param name="enableLighting">ライティングを有効にするか</param>
    void CreateModel(
        ID3D12Device *device,
        const std::string &fileName,
        uint32_t color, bool enableLighting
    );

    /// <summary>
    /// ワールド行列とWVP行列を更新する
    /// </summary>
    /// <param name="camera">使用するカメラ</param>
    /// <param name="transform">オブジェクトのTransform</param>
    void Update(Camera *camera, Transform transform);

    /// <summary>
    /// 通常描画を行う
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="texture">使用するテクスチャ</param>
    void Draw(ID3D12GraphicsCommandList *commandList, TextureData &texture);

    /// <summary>
    /// 壁越し描画を行う
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="texture">使用するテクスチャ</param>
    void DrawThroughWall(ID3D12GraphicsCommandList *commandList, TextureData &texture);

    //=====================================================================
    // Setter
    //=====================================================================

    /// <summary>
    /// 描画レイヤを変更する
    /// </summary>
    /// <param name="layer">設定する描画レイヤ</param>
    void SetRenderLayer(RenderLayer layer) { renderLayer_ = layer; }

    //=====================================================================
    // Getter
    //=====================================================================

    /// <summary>
    /// モデルデータを取得する
    /// </summary>
    /// <returns>モデルデータ</returns>
    ModelData &GetModelData() { return modelData_; }

    /// <summary>
    /// 通常描画用マテリアルを取得する
    /// </summary>
    /// <returns>通常描画用マテリアル</returns>
    Material &GetMaterial() { return material_; }

    /// <summary>
    /// 壁越し描画用マテリアルを取得する
    /// </summary>
    /// <returns>壁越し描画用マテリアル</returns>
    Material &GetThroughWallMaterial() { return throughWallMaterial_; }

    /// <summary>
    /// 現在の描画レイヤを取得する
    /// </summary>
    /// <returns>現在設定されている描画レイヤ</returns>
    RenderLayer GetRenderLayer() const { return renderLayer_; }

private:
    // 読み込んだモデルデータ
    ModelData modelData_;

    // 描画データ生成補助クラス
    Renderer render_;

    // 通常描画用マテリアル
    Material material_;

    // 壁越し描画用マテリアル
    Material throughWallMaterial_;

    // 描画に使用するメッシュ
    Mesh mesh_;

    // プリミティブ生成用頂点データ
    std::vector<VertexData> vertices_;

    // インデックスバッファ用データ
    std::vector<uint32_t> indices_;

    // WVP・World行列用定数バッファ
    MeshBuffer transformationMatrixBuffer_;

    // GPUへ送信する行列データ
    TransformationMatrix *transformationMatrixData_ = nullptr;

    // 描画方法を切り替えるためのレイヤ
    RenderLayer renderLayer_ = RenderLayer::kDefault;
};