#include "Sprite.h"

#include "Math/Functions.h"

void Sprite::Initialize(
	ID3D12Device *device, 
	float left, float top, float right, float bottom,
	uint32_t color
) {
	// 頂点生成
	render_.CreateSprite(vertices_, indices_, left, top, right, bottom);

	// 頂点データ
	render_.CreateVertexBuffer(device, vertexBuffer_, vertices_);

	// 頂点インデックス
	render_.CreateIndexBuffer(device, indexBuffer_, indices_);

	// TransformationMatrix
	render_.CreateTransformationMatrixBuffer(device, transformationMatrixBuffer_, transformationMatrixData_);

	// Sprite用
	material_.Initialize(device, color, false);
}

void Sprite::Update(uint32_t width, uint32_t height) {
	// カメラ処理
	Matrix4x4 worldMatrix = MakeWorldMatrix(transform_);

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(
		0.0f, 0.0f, float(width), float(height), 0.0f, 100.0f);
	
	Matrix4x4 worldViewProjectionMatrix = 
		Multiply(Multiply(worldMatrix, viewMatrix), projectionMatrixSprite);

	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;

	// uvTransform
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform_.scale);

	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform_.rotate.z));

	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform_.translate));
	
	material_.GetMaterialData()->uvTransform = uvTransformMatrix;
}

void Sprite::Draw(ID3D12GraphicsCommandList *commandList, Texture &texture) {
	commandList->SetGraphicsRootConstantBufferView(0, material_.GetGPUVirtualAddress());

	// TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixBuffer_.GetGPUVirtualAddress());
	
	// テクスチャ決定
	commandList->SetGraphicsRootDescriptorTable(2, texture.GetGPUHandle());

	// Spriteの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, vertexBuffer_.GetVertexBufferView());
	commandList->IASetIndexBuffer(indexBuffer_.GetIndexBufferView());	// IBVを設定する

	// 描画!(DrawCall/ドローコール)
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}