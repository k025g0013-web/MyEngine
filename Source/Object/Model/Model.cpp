#include "Model.h"

#include <cassert>
#include <filesystem>

#include "Math/FunctionMatrix.h"

namespace Kizuna {

    void Model::Create(
        ID3D12Device *device,
        ID3D12GraphicsCommandList *commandList,
        uint32_t color,
        bool enableLighting) {

        // モデルを読み込む
        modelData_ =
            ModelLoader::LoadObjFile(fileName_);

        // メッシュを生成
        mesh_.Create(
            device,
            modelData_.vertices);

        // 外部テクスチャを指定していない場合、
        // モデルと同名のテクスチャを読み込む
        if (!useExternalTexture_) {

            assert(textureManager_ != nullptr);

            std::string texturePath =
                FindTexturePath();

            if (!texturePath.empty()) {

                textureData_ =
                    textureManager_->LoadTexture(
                        commandList,
                        texturePath);
            }
        }

        // 行列用定数バッファを生成
        renderer_.CreateTransformationMatrixBuffer(
            device,
            transformationMatrixBuffer_,
            transformationMatrixData_);

        // マテリアルを初期化
        material_.Initialize(
            device,
            color,
            enableLighting);
    }

    void Model::Update(
        CameraManager *camera,
        Transform transform) {

        Matrix4x4 worldMatrix =
            Math::MakeAffineMatrix(
                transform.scale,
                transform.rotate,
                transform.translate);

        Matrix4x4 wvp =
            Math::Multiply(
                worldMatrix,
                camera->GetViewProjectionMatrix());

        transformationMatrixData_->World =
            worldMatrix;

        transformationMatrixData_->WVP =
            wvp;

        Matrix4x4 uvTransformMatrix{};

        uvTransformMatrix =
            Math::MakeScaleMatrix(
                uvTransform_.scale);

        uvTransformMatrix =
            Math::Multiply(
                uvTransformMatrix,
                Math::MakeRotateZMatrix(
                    uvTransform_.rotate.z));

        uvTransformMatrix =
            Math::Multiply(
                uvTransformMatrix,
                Math::MakeTranslateMatrix(
                    uvTransform_.translate));

        material_.GetMaterialData()->uvTransform =
            uvTransformMatrix;
    }

    void Model::Draw(
        ID3D12GraphicsCommandList *commandList) {

        commandList->SetGraphicsRootConstantBufferView(
            0,
            material_.GetGPUVirtualAddress());

        commandList->SetGraphicsRootConstantBufferView(
            1,
            transformationMatrixBuffer_.GetGPUVirtualAddress());

        // テクスチャが存在する場合のみ設定
        if (!textureData_.gpuHandle.ptr == false) {

            commandList->SetGraphicsRootDescriptorTable(
                2,
                textureData_.gpuHandle);
        }

        mesh_.Bind(commandList);
    }

    std::string Model::FindTexturePath() const {

        const std::string directory =
            "Resources/Models/" + fileName_ + "/";

        const std::string extensions[] = {
            ".png",
            ".jpg",
            ".jpeg"
        };

        for (const auto &extension : extensions) {

            std::filesystem::path path =
                directory + fileName_ + extension;

            if (std::filesystem::exists(path)) {
                return path.string();
            }
        }

        return "";
    }
}