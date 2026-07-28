#pragma once

#include <string>

#include "Asset/ModelLoader.h"

#include "Graphics/Resource/MeshBuffer.h"
#include "Graphics/Resource/Texture.h"
#include "RenderCore/Camera/CameraManager.h"
#include "RenderCore/Material.h"
#include "RenderCore/Mesh.h"
#include "Renderer/Renderer.h"

namespace Kizuna {
    enum class RenderPass {
        kDefault,
        kThroughWall,
        kShadowMap,
        kDepth,
        kOutline,
    };

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
        virtual ~Object3D() = default;

        /// <summary>
        /// 各形状固有の生成処理
        /// </summary>
        virtual void Create(
            ID3D12Device *device, uint32_t color, bool enableLighting) = 0;

        /// <summary>
        /// ワールド行列とWVP行列を更新する
        /// </summary>
        /// <param name="camera">使用するカメラ</param>
        /// <param name="transform">オブジェクトのTransform</param>
        virtual void Update(
            CameraManager *camera, Transform transform);

        /// <summary>
        /// 通常描画を行う
        /// </summary>
        /// <param name="commandList">コマンドリスト</param>
        /// <param name="texture">使用するテクスチャ</param>
        virtual void Draw(
            ID3D12GraphicsCommandList *commandList, TextureData &texture);

        /// 共通壁越し描画
        virtual void DrawThroughWall(
            ID3D12GraphicsCommandList *commandList);

        /// <summary>
        /// マテリアルを取得する
        /// </summary>
        /// <returns>マテリアル</returns>
        Material &GetMaterial() { return material_; }

        /// <summary>
        /// 壁越し描画用マテリアルを取得する
        /// </summary>
        /// <returns>壁越し描画用マテリアル</returns>
        ThroughWallMaterial &GetThroughWallMaterial() { return throughWallMaterial_; }

        /// <summary>
        /// オブジェクト用Transformを取得する
        /// </summary>
        /// <returns>オブジェクト用Transform</returns>
        Transform &GetTransform() { return transform_; }

        /// <summary>
        /// UV座標用Transformを取得する
        /// </summary>
        /// <returns>UV座標用Transform</returns>
        Transform &GetUVTransform() { return uvTransform_; }

    protected:
        /// 派生クラスから呼ぶ共通初期化
        void InitializeMaterial(
            ID3D12Device *device, uint32_t color, bool enableLighting);

    protected:
        // 描画データ生成補助クラス
        Renderer renderer_;

        // 描画に使用するメッシュ
        Mesh mesh_;

        // 通常描画用マテリアル
        Material material_;

        // 壁越し描画用マテリアル
        ThroughWallMaterial throughWallMaterial_;

        // 読み込んだモデルデータ
        ModelData modelData_;

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

        // オブジェクト用Transform
        Transform transform_{
            {1,1,1}, {0,0,0}, {0,0,0}
        };

        // UV座標用Transform
        Transform uvTransform_{
            {1,1,1}, {0,0,0}, {0,0,0}
        };
    };
}