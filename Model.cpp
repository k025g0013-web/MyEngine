#include "Model.h"

#include "MathFunctions.h"

void Model::Initialize(
	ID3D12Device *device,
	const std::string &directoryPath,
	const std::string &fileName,
	uint32_t color, bool enableLighting
) {
	// モデル読み込み
	modelData_ = ModelLoader::LoadObjFile(directoryPath, fileName);

	// 頂点データ
	render_.CreateVertexBuffer(device, vertexBuffer_, modelData_.vertices);

	// TransformationMatrix
	render_.CreateTransformationMatrixBuffer(
		device, transformationMatrixBuffer_, transformationMatrixData_
	);

	// マテリアル
	material_.Initialize(device, color, enableLighting);
}

void Model::Update(Camera *camera) {
	Matrix4x4 worldMatrix = MakeWorldMatrix(transform_);

	Matrix4x4 worldViewProjectionMatrix =
		Multiply(worldMatrix, camera->GetViewProjectionMatrix());

	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

void Model::Draw(ID3D12GraphicsCommandList *commandList, Texture &texture) {
	commandList->IASetVertexBuffers(0, 1, &vertexBuffer_.GetView());

	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(
		0, material_.GetGPUVirtualAddress());

	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(
		1, transformationMatrixBuffer_.GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定
	commandList->SetGraphicsRootDescriptorTable(
		2, texture.GetGPUHandle());

	// 描画!(DrawCall/ドローコール)
	commandList->DrawInstanced(
		UINT(modelData_.vertices.size()), 1, 0, 0);
}