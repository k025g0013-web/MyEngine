#include "Object3D.h"

#include "Math/FunctionMatrix.h"

namespace Kizuna {
	void Object3D::InitializeMaterial(
		ID3D12Device *device, uint32_t color, bool enableLighting) {
		// WVP・World行列用定数バッファを生成する
		renderer_.CreateTransformationMatrixBuffer(
			device, transformationMatrixBuffer_, transformationMatrixData_);

		// 通常描画用マテリアルを生成する
		material_.Initialize(device, color, enableLighting);

		// 通常描画用マテリアルを生成する
		throughWallMaterial_.Initialize(device, color);
	}

	void Object3D::Update(
		CameraManager *camera, Transform transform) {
		// ワールド行列を生成する
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
			transform.scale, transform.rotate, transform.translate);

		// WVP行列を生成する
		Matrix4x4 wvp = Math::Multiply(
				worldMatrix, camera->GetViewProjectionMatrix());

		// GPUへ行列を書き込む
		transformationMatrixData_->World = worldMatrix;
		transformationMatrixData_->WVP = wvp;

		// UV座標の変換行列を生成する
		Matrix4x4 uvTransformMatrix{};
		uvTransformMatrix = Math::MakeScaleMatrix(uvTransform_.scale);												// UVの拡大縮小
		uvTransformMatrix = Math::Multiply(uvTransformMatrix, Math::MakeRotateZMatrix(uvTransform_.rotate.z));		// UVの回転
		uvTransformMatrix = Math::Multiply(uvTransformMatrix, Math::MakeTranslateMatrix(uvTransform_.translate));	// UVの平行移動

		// 更新したUV変換行列をマテリアルへ反映する
		material_.GetMaterialData()->uvTransform = uvTransformMatrix;
	}

	void Object3D::Draw(
		ID3D12GraphicsCommandList *commandList, TextureData &texture) {
		// 通常描画用マテリアルを設定する
		commandList->SetGraphicsRootConstantBufferView(
			0, material_.GetGPUVirtualAddress());

		// WVP・World行列を設定する
		commandList->SetGraphicsRootConstantBufferView(
			1, transformationMatrixBuffer_.GetGPUVirtualAddress());

		// 描画に使用するテクスチャを設定する
		commandList->SetGraphicsRootDescriptorTable(
			2, texture.gpuHandle);

		// メッシュを描画する
		mesh_.Bind(commandList);
	}

	void Object3D::DrawThroughWall(
		ID3D12GraphicsCommandList *commandList) {
		// 壁越し描画専用マテリアルを設定する
		commandList->SetGraphicsRootConstantBufferView(
			0, throughWallMaterial_.GetGPUVirtualAddress());

		// WVP・World行列を設定する
		commandList->SetGraphicsRootConstantBufferView(
			1, transformationMatrixBuffer_.GetGPUVirtualAddress());

		// メッシュを描画する
		mesh_.Bind(commandList);
	}
}