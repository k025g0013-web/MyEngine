#include "Sprite.h"
#include "Math/FunctionMatrix.h"

namespace Kizuna {
	void Sprite::Initialize(
		ID3D12Device *device,
		float left, float top, float right, float bottom,
		uint32_t color
	) {
		// 指定された矩形サイズからスプライトの頂点情報を生成する
		render_.CreateSprite(vertices_, indices_, left, top, right, bottom);

		// GPUへ転送する頂点バッファを生成する
		render_.CreateVertexBuffer(device, vertexBuffer_, vertices_);

		// インデックスバッファを生成する
		render_.CreateIndexBuffer(device, indexBuffer_, indices_);

		// ワールド行列・射影行列を保持する定数バッファを生成する
		render_.CreateTransformationMatrixBuffer(device, transformationMatrixBuffer_, transformationMatrixData_);

		// スプライト専用マテリアルを生成する
		material_.Initialize(device, color, false);
	}

	void Sprite::Update(uint32_t width, uint32_t height) {
		// スプライトの位置・回転・拡大縮小からワールド行列を生成する
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

		// スプライトは画面座標系で描画するためビュー行列は単位行列を使用する
		Matrix4x4 viewMatrix = Math::MakeIdentity<Matrix4x4>();

		// ピクセル座標で描画するため正射影行列を生成する
		Matrix4x4 projectionMatrixSprite = Math::MakeOrthographicMatrix(
			0.0f, 0.0f, float(width), float(height), 0.0f, 100.0f);

		// ワールド→ビュー→射影の順に合成する
		Matrix4x4 worldViewProjectionMatrix =
			Math::Multiply(Math::Multiply(worldMatrix, viewMatrix), projectionMatrixSprite);

		// GPUへ描画行列を転送する
		transformationMatrixData_->WVP = worldViewProjectionMatrix;
		transformationMatrixData_->World = worldMatrix;

		// UV座標の変換行列を生成する
		Matrix4x4 uvTransformMatrix{};
		uvTransformMatrix = Math::MakeScaleMatrix(uvTransform_.scale);												// UVの拡大縮小
		uvTransformMatrix = Math::Multiply(uvTransformMatrix, Math::MakeRotateZMatrix(uvTransform_.rotate.z));		// UVの回転
		uvTransformMatrix = Math::Multiply(uvTransformMatrix, Math::MakeTranslateMatrix(uvTransform_.translate));	// UVの平行移動

		// 更新したUV変換行列をマテリアルへ反映する
		material_.GetMaterialData()->uvTransform = uvTransformMatrix;
	}

	void Sprite::Draw(ID3D12GraphicsCommandList *commandList, TextureData &texture) {
		// マテリアル定数バッファを設定
		commandList->SetGraphicsRootConstantBufferView(0, material_.GetGPUVirtualAddress());

		// 変換行列用の定数バッファをシェーダへ設定する
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixBuffer_.GetGPUVirtualAddress());

		// 描画に使用するテクスチャをバインド
		commandList->SetGraphicsRootDescriptorTable(2, texture.gpuHandle);

		// スプライト用の頂点を設定
		commandList->IASetVertexBuffers(0, 1, vertexBuffer_.GetVertexBufferView());
		// インデックスバッファを設定する
		commandList->IASetIndexBuffer(indexBuffer_.GetIndexBufferView());

		// 2枚の三角形で構成される矩形を描画する
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}