#include "Object3D.h"

#include "Math/FunctionMatrix.h"

void Object3D::CreatePlaneTriangle(		// 平面三角形
	ID3D12Device *device,
	Vector3 left, Vector3 top, Vector3 right,
	uint32_t color, bool enableLighting
) {
	// モデル
	render_.CreateTriangle(vertices_, left, top, right);

	// 頂点データ
	mesh_.Create(device, vertices_);

	// TransformationMatrix
	render_.CreateTransformationMatrixBuffer(
		device, transformationMatrixBuffer_, transformationMatrixData_);

	// マテリアル
	material_.Initialize(device, color, enableLighting);
}

void Object3D::CreateSphere(			// 球
	ID3D12Device *device,
	uint32_t subdivision,
	uint32_t color, bool enableLighting
) {
	// モデル
	render_.CreateSphere(vertices_, indices_, subdivision);

	// 頂点データ
	mesh_.Create(device, vertices_, indices_);

	// TransformationMatrix
	render_.CreateTransformationMatrixBuffer(
		device, transformationMatrixBuffer_, transformationMatrixData_);

	// マテリアル
	material_.Initialize(device, color, enableLighting);
}

void Object3D::CreateModel(				// モデル
	ID3D12Device *device,
	const std::string &fileName,
	uint32_t color, bool enableLighting
) {
	// モデル読み込み
	modelData_ = ModelLoader::LoadObjFile(fileName);

	// 頂点データ
	mesh_.Create(device, modelData_.vertices);

	// TransformationMatrix
	render_.CreateTransformationMatrixBuffer(
		device, transformationMatrixBuffer_, transformationMatrixData_
	);

	// マテリアル
	material_.Initialize(device, color, enableLighting);
}

void Object3D::Update(Camera *camera, Transform transform) {
	Matrix4x4 worldMatrix = Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	Matrix4x4 worldViewProjectionMatrix =
		Math::Multiply(worldMatrix, camera->GetViewProjectionMatrix());

	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

void Object3D::Draw(ID3D12GraphicsCommandList *commandList, TextureData &texture) {
	// マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, material_.GetGPUVirtualAddress());

	// wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixBuffer_.GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定
    commandList->SetGraphicsRootDescriptorTable(2, texture.gpuHandle);

    mesh_.Bind(commandList);
}