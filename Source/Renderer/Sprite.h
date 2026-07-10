#pragma once
#include <vector>
#include "Graphics/Resource/MeshBuffer.h"
#include "Graphics/Resource/Texture.h"
#include "RenderCore/Camera.h"
#include "RenderCore/Material.h"
#include "Renderer/Renderer.h"
#include "Math/Transform.h"

/// <summary>
/// 2Dスプライトを描画するクラス
/// </summary>
/// <remarks>
/// 頂点バッファやマテリアル、変換行列を管理し、
/// 画面上へ2D画像を描画する。
/// UIやHUDなどの2Dオブジェクトで利用する。
/// </remarks>
class Sprite {
public:
	/// <summary>
	/// スプライト描画に必要なリソースを初期化する
	/// </summary>
	/// <param name="device">
	/// Direct3Dデバイス
	/// </param>
	/// <param name="left">
	/// 左端座標
	/// </param>
	/// <param name="top">
	/// 上端座標
	/// </param>
	/// <param name="right">
	/// 右端座標
	/// </param>
	/// <param name="bottom">
	/// 下端座標
	/// </param>
	/// <param name="color">
	/// 初期カラー（0xRRGGBBAA形式）
	/// </param>
	void Initialize(
		ID3D12Device *device,
		float left, float top, float right, float bottom,
		uint32_t color
	);

	/// <summary>
	/// スプライトの変換行列およびUV変換を更新する
	/// </summary>
	/// <param name="windowWidth">
	/// ウィンドウ横幅
	/// </param>
	/// <param name="windowHeight">
	/// ウィンドウ縦幅
	/// </param>
	/// <remarks>
	/// ワールド行列・射影行列を生成し、
	/// 定数バッファへ反映する。
	/// またUVスクロールや拡大縮小も更新する。
	/// </remarks>
	void Update(uint32_t windowWidth, uint32_t windowHeight);

	/// <summary>
	/// スプライトを描画する
	/// </summary>
	/// <param name="commandList">
	/// 描画コマンドリスト
	/// </param>
	/// <param name="texture">
	/// 描画に使用するテクスチャ
	/// </param>
	void Draw(ID3D12GraphicsCommandList *commandList, TextureData &texture);

	// getter
	Transform &GetTransform() { return transform_; }
	Transform &GetUVTransform() { return uvTransform_; }
	Material &GetMaterial() { return material_; }

private:
	// 描画処理共通クラス
	Renderer render_;

	// スプライト頂点
	std::vector<VertexData> vertices_;
	// インデックス
	std::vector<uint32_t> indices_;

	// 頂点バッファ
	MeshBuffer vertexBuffer_;
	// インデックスバッファ
	MeshBuffer indexBuffer_;

	// Transform用定数バッファ
	MeshBuffer transformationMatrixBuffer_;
	// GPUへ転送する変換行列
	TransformationMatrix *transformationMatrixData_ = nullptr;

	// マテリアル情報
	Material material_;

	// スプライト本体のTransform
	Transform transform_{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	// UV座標用Transform
	Transform uvTransform_{
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	};
};
