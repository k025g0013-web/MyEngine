#include "Object3D.h"

#include "Math/FunctionMatrix.h"

void Object3D::CreatePlaneTriangle(
	ID3D12Device *device,
	Vector3 left, Vector3 top, Vector3 right,
	uint32_t color, bool enableLighting
) {
	// 三角形の頂点データを生成する
	render_.CreateTriangle(vertices_, left, top, right);

	// メッシュを生成する
	mesh_.Create(device, vertices_);

	// WVP・World行列用定数バッファを生成する
	render_.CreateTransformationMatrixBuffer(
		device, transformationMatrixBuffer_, transformationMatrixData_);

	// 通常描画用マテリアルを生成する
	material_.Initialize(device, color, enableLighting);

	// 壁越し描画用マテリアルも同時に生成する
	throughWallMaterial_.Initialize(device, color);
}

void Object3D::CreateSphere(
	ID3D12Device *device,
	uint32_t subdivision,
	uint32_t color, bool enableLighting
) {
	// 球体の頂点・インデックスを生成する
	render_.CreateSphere(vertices_, indices_, subdivision);

	// 球体メッシュを生成する
	mesh_.Create(device, vertices_, indices_);

	// WVP・World行列用定数バッファを生成する
	render_.CreateTransformationMatrixBuffer(
		device, transformationMatrixBuffer_, transformationMatrixData_);

	// 通常描画用マテリアルを生成する
	material_.Initialize(device, color, enableLighting);

	// 壁越し描画用マテリアルも生成する
	throughWallMaterial_.Initialize(device, color);
}

void Object3D::CreateModel(
	ID3D12Device *device,
	const std::string &fileName,
	uint32_t color, bool enableLighting
) {
	// objモデルを読み込む
	modelData_ = ModelLoader::LoadObjFile(fileName);

	// 読み込んだ頂点データからメッシュを生成する
	mesh_.Create(device, modelData_.vertices);

	// WVP・World行列用定数バッファを生成する
	render_.CreateTransformationMatrixBuffer(
		device, transformationMatrixBuffer_, transformationMatrixData_
	);

	// 通常描画用マテリアルを生成する
	material_.Initialize(device, color, enableLighting);

	// 壁越し描画用マテリアルも生成する
	throughWallMaterial_.Initialize(device, color);
}

void Object3D::Update(Camera *camera, Transform transform) {
	// ワールド行列を生成する
	Matrix4x4 worldMatrix =
		Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	// WVP行列を生成する
	Matrix4x4 worldViewProjectionMatrix =
		Math::Multiply(worldMatrix, camera->GetViewProjectionMatrix());

	// GPUへ行列を書き込む
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

void Object3D::Draw(ID3D12GraphicsCommandList *commandList, TextureData &texture) {
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

void Object3D::DrawThroughWall(ID3D12GraphicsCommandList *commandList, TextureData &texture) {
	// 壁越し描画専用マテリアルを設定する
	commandList->SetGraphicsRootConstantBufferView(
		0, throughWallMaterial_.GetGPUVirtualAddress());

	// WVP・World行列を設定する
	commandList->SetGraphicsRootConstantBufferView(
		1, transformationMatrixBuffer_.GetGPUVirtualAddress());

	// 描画に使用するテクスチャを設定する
	commandList->SetGraphicsRootDescriptorTable(
		2, texture.gpuHandle);

	// メッシュを描画する
	mesh_.Bind(commandList);
}