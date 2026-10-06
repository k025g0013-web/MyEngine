#pragma once

#include <string>
#include <utility>

#include "ModelLoader.h"

#include "Graphics/Material/Material.h"
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Resource/Mesh.h"
#include "Graphics/Resource/MeshBuffer.h"
#include "Graphics/Resource/Texture.h"
#include "Graphics/Resource/TransformationMatrix.h"

#include "Manager/TextureManager.h"

#include "RenderCore/Camera/CameraManager.h"

namespace Kizuna {

    /// <summary>
    /// 3Dモデルを管理・描画するクラス
    /// </summary>
    class Model {
    public:

        /// <summary>
        /// モデル付属のテクスチャを使用するモデル
        /// </summary>
        Model(
            std::string fileName,
            TextureManager *textureManager)
            : fileName_(std::move(fileName)),
            textureManager_(textureManager) {}

        /// <summary>
        /// 外部から指定したテクスチャを使用するモデル
        /// </summary>
        Model(
            std::string fileName,
            TextureManager *textureManager,
            const TextureData &textureData)
            : fileName_(std::move(fileName)),
            textureManager_(textureManager),
            textureData_(textureData),
            useExternalTexture_(true) {}

        /// <summary>
        /// モデルを生成する
        /// </summary>
        void Create(
            ID3D12Device *device,
            ID3D12GraphicsCommandList *commandList,
            uint32_t color,
            bool enableLighting);

        /// <summary>
        /// ワールド行列とWVP行列を更新する
        /// </summary>
        void Update(
            CameraManager *camera,
            Transform transform);

        /// <summary>
        /// モデルを描画する
        /// </summary>
        void Draw(
            ID3D12GraphicsCommandList *commandList);

        /// <summary>
        /// モデルのTransformを取得する
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

        /// <summary>
        /// マテリアルを取得する
        /// </summary>
        Material &GetMaterial() {
            return material_;
        }

    private:

        /// <summary>
        /// モデルと同名のテクスチャファイルを探す
        /// </summary>
        std::string FindTexturePath() const;

    private:

        // モデルファイル名
        std::string fileName_;

        // モデルデータ
        ModelData modelData_;

        // テクスチャ管理
        TextureManager *textureManager_ = nullptr;

        // 使用するテクスチャ
        TextureData textureData_{};

        // 外部テクスチャを使用するか
        bool useExternalTexture_ = false;

        // GPUリソース生成
        Renderer renderer_;

        // 描画に使用するメッシュ
        Mesh mesh_;

        // 通常描画用マテリアル
        Material material_;

        // WVP・World行列用定数バッファ
        MeshBuffer transformationMatrixBuffer_;

        // GPUへ送信する行列データ
        TransformationMatrix *transformationMatrixData_ = nullptr;

        // モデル用Transform
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