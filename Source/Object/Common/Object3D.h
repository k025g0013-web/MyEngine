#pragma once

#include "Graphics/Material/Material.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Renderer/MeshGenerator.h"
#include "Graphics/Resource/Mesh.h"
#include "Graphics/Resource/MeshBuffer.h"
#include "Graphics/Resource/TransformationMatrix.h"
#include "RenderCore/Camera/CameraManager.h"

namespace Kizuna {

    /// <summary>
    /// 描画方法を切り替えるためのレンダーレイヤ
    /// </summary>
    enum class RenderLayer {
        kDefault,      // 通常描画
        kThroughWall   // 壁越し描画対象
    };

    /// <summary>
    /// 3Dオブジェクト共通の基底クラス
    /// </summary>
    class Object3D {
    public:
        virtual ~Object3D() = default;

        /// <summary>
        /// 各オブジェクト固有の生成処理
        /// </summary>
        virtual void Create(
            ID3D12Device *device,
            ID3D12GraphicsCommandList *commandList,
            uint32_t color,
            bool enableLighting
        ) = 0;

        /// <summary>
        /// ワールド行列とWVP行列を更新する
        /// </summary>
        virtual void Update(
            CameraManager *camera,
            Transform transform
        );

        /// <summary>
        /// 通常描画を行う
        /// </summary>
        virtual void Draw(
            ID3D12GraphicsCommandList *commandList
        );

        /// <summary>
        /// 壁越し描画を行う
        /// </summary>
        virtual void DrawThroughWall(
            ID3D12GraphicsCommandList *commandList
        );

        /// <summary>
        /// 通常描画用マテリアルを取得する
        /// </summary>
        Material &GetMaterial() {
            return material_;
        }

        /// <summary>
        /// 壁越し描画用マテリアルを取得する
        /// </summary>
        ThroughWallMaterial &GetThroughWallMaterial() {
            return throughWallMaterial_;
        }

        /// <summary>
        /// オブジェクト用Transformを取得する
        /// </summary>
        Transform &GetTransform() {
            return transform_;
        }

        /// <summary>
        /// UV座標用Transformを取得する
        /// </summary>
        Transform &GetUVTransform() {
            return uvTransform_;
        }

    protected:
        /// <summary>
        /// 3Dオブジェクト共通の初期化処理
        /// </summary>
        void InitializeMaterial(
            ID3D12Device *device,
            uint32_t color,
            bool enableLighting
        );

    protected:
        // GPUリソース生成
        Renderer renderer_;

        // CPU上のメッシュデータ生成
        MeshGenerator meshGenerator_;

        // 描画に使用するメッシュ
        Mesh mesh_;

        // 通常描画用マテリアル
        Material material_;

        // 壁越し描画用マテリアル
        ThroughWallMaterial throughWallMaterial_;

        // WVP・World行列用定数バッファ
        MeshBuffer transformationMatrixBuffer_;

        // GPUへ送信する行列データ
        TransformationMatrix *transformationMatrixData_ = nullptr;

        // 描画方法を切り替えるためのレイヤ
        RenderLayer renderLayer_ = RenderLayer::kDefault;

        // オブジェクト用Transform
        Transform transform_{
            {1, 1, 1},
            {0, 0, 0},
            {0, 0, 0}
        };

        // UV座標用Transform
        Transform uvTransform_{
            {1, 1, 1},
            {0, 0, 0},
            {0, 0, 0}
        };
    };
}