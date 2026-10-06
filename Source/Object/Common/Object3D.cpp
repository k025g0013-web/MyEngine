#include "Object3D.h"

#include "Math/FunctionMatrix.h"

namespace Kizuna {

    void Object3D::InitializeMaterial(
        ID3D12Device *device,
        uint32_t color,
        bool enableLighting) {

        renderer_.CreateTransformationMatrixBuffer(
            device,
            transformationMatrixBuffer_,
            transformationMatrixData_);

        material_.Initialize(
            device,
            color,
            enableLighting);
        /*
        throughWallMaterial_.Initialize(
            device,
            color);
        */
    }

    void Object3D::Update(
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

        transformationMatrixData_->World = worldMatrix;
        transformationMatrixData_->WVP = wvp;

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

    void Object3D::Draw(
        ID3D12GraphicsCommandList *commandList) {

        commandList->SetGraphicsRootConstantBufferView(
            0,
            material_.GetGPUVirtualAddress());

        commandList->SetGraphicsRootConstantBufferView(
            1,
            transformationMatrixBuffer_.GetGPUVirtualAddress());

        assert(
            textureData_.gpuHandle.ptr != 0 &&
            "Object3Dに有効なテクスチャが設定されていません");

        commandList->SetGraphicsRootDescriptorTable(
            2,
            textureData_.gpuHandle);

        mesh_.Bind(commandList);
    }

    /*
    void Object3D::DrawThroughWall(
        ID3D12GraphicsCommandList *commandList) {

        commandList->SetGraphicsRootConstantBufferView(
            0,
            throughWallMaterial_.GetGPUVirtualAddress());

        commandList->SetGraphicsRootConstantBufferView(
            1,
            transformationMatrixBuffer_.GetGPUVirtualAddress());

        mesh_.Bind(commandList);
    }
    */
}