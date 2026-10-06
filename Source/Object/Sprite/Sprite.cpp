#include "Sprite.h"

#include <vector>

#include "Math/FunctionMatrix.h"

namespace Kizuna {

    void Sprite::Initialize(
        ID3D12Device *device,
        float left,
        float top,
        float right,
        float bottom,
        uint32_t color) {

        // スプライトの頂点・インデックスを生成する
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;

        meshGenerator_.CreateSprite(
            vertices,
            indices,
            left,
            top,
            right,
            bottom
        );

        // スプライトのメッシュを生成する
        mesh_.Create(
            device,
            vertices,
            indices
        );

        // 変換行列用定数バッファを生成する
        // SpriteではRendererを直接使用せず、
        // ここは今まで通りMeshBufferを直接生成するか、
        // 後ほど共通化してもよい
        transformationMatrixBuffer_.InitializeAsConstant(
            device,
            sizeof(TransformationMatrix)
        );

        transformationMatrixData_ =
            static_cast<TransformationMatrix *>(
                transformationMatrixBuffer_.Map()
                );

        transformationMatrixData_->WVP =
            Math::MakeIdentity<Matrix4x4>();

        transformationMatrixData_->World =
            Math::MakeIdentity<Matrix4x4>();

        // マテリアルを生成する
        material_.Initialize(
            device,
            color,
            false
        );
    }

    void Sprite::Update(
        uint32_t width,
        uint32_t height) {

        // スプライトの位置・回転・拡大縮小から
        // ワールド行列を生成する
        Matrix4x4 worldMatrix =
            Math::MakeAffineMatrix(
                transform_.scale,
                transform_.rotate,
                transform_.translate
            );

        // スプライトは画面座標系で描画するため
        // ビュー行列は単位行列を使用する
        Matrix4x4 viewMatrix =
            Math::MakeIdentity<Matrix4x4>();

        // ピクセル座標で描画するため
        // 正射影行列を生成する
        Matrix4x4 projectionMatrixSprite =
            Math::MakeOrthographicMatrix(
                0.0f,
                0.0f,
                static_cast<float>(width),
                static_cast<float>(height),
                0.0f,
                100.0f
            );

        // ワールド → ビュー → 射影
        Matrix4x4 worldViewProjectionMatrix =
            Math::Multiply(
                Math::Multiply(
                    worldMatrix,
                    viewMatrix
                ),
                projectionMatrixSprite
            );

        // GPUへ描画行列を反映する
        transformationMatrixData_->WVP =
            worldViewProjectionMatrix;

        transformationMatrixData_->World =
            worldMatrix;

        // UV変換行列を生成する
        Matrix4x4 uvTransformMatrix{};

        uvTransformMatrix =
            Math::MakeScaleMatrix(
                uvTransform_.scale
            );

        uvTransformMatrix =
            Math::Multiply(
                uvTransformMatrix,
                Math::MakeRotateZMatrix(
                    uvTransform_.rotate.z
                )
            );

        uvTransformMatrix =
            Math::Multiply(
                uvTransformMatrix,
                Math::MakeTranslateMatrix(
                    uvTransform_.translate
                )
            );

        // UV変換をマテリアルへ反映する
        material_.GetMaterialData()->uvTransform =
            uvTransformMatrix;
    }

    void Sprite::Draw(
        ID3D12GraphicsCommandList *commandList,
        TextureData &texture) {

        // マテリアル
        commandList->SetGraphicsRootConstantBufferView(
            0,
            material_.GetGPUVirtualAddress()
        );

        // WVP
        commandList->SetGraphicsRootConstantBufferView(
            1,
            transformationMatrixBuffer_.GetGPUVirtualAddress()
        );

        // テクスチャ
        commandList->SetGraphicsRootDescriptorTable(
            2,
            texture.gpuHandle
        );

        // メッシュを描画
        mesh_.Bind(commandList);
    }
}