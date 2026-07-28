#pragma once
#include <vector>
#include "Graphics/Resource/MeshBuffer.h"
#include "RenderCore/TransformationMatrix.h"

namespace Kizuna {
	/// <summary>
	/// 描画用メッシュデータおよびGPUリソースを生成する補助クラス
	/// </summary>
	/// <remarks>
	/// 頂点バッファ・インデックスバッファ・定数バッファの生成や、
	/// スプライト・三角形・球などの基本形状の頂点データ生成を担当する。
	/// 描画処理とリソース生成処理を分離し、各描画クラスから共通利用できるようにしている。
	/// </remarks>
	class Renderer {
	public:
		/// <summary>
		/// 頂点データからGPU用頂点バッファを生成する
		/// </summary>
		/// <param name="device">Direct3Dデバイス</param>
		/// <param name="vertexBuffer">生成先の頂点バッファ</param>
		/// <param name="vertices">頂点データ配列</param>
		void CreateVertexBuffer(
			ID3D12Device *device,
			MeshBuffer &vertexBuffer,
			const std::vector<VertexData> &vertices
		);

		/// <summary>
		/// インデックスデータからGPU用インデックスバッファを生成する
		/// </summary>
		/// <param name="device">Direct3Dデバイス</param>
		/// <param name="indexBuffer">生成先のインデックスバッファ</param>
		/// <param name="indices">インデックスデータ配列</param>
		void CreateIndexBuffer(
			ID3D12Device *device,
			MeshBuffer &indexBuffer,
			const std::vector<uint32_t> &indices
		);

		/// <summary>
		/// ワールド行列・WVP行列を格納する定数バッファを生成する
		/// </summary>
		/// <param name="device">Direct3Dデバイス</param>
		/// <param name="constantBuffer">生成先の定数バッファ</param>
		/// <param name="data">マッピング先のCPUポインタ</param>
		void CreateTransformationMatrixBuffer(
			ID3D12Device *device,
			MeshBuffer &constantBuffer,
			TransformationMatrix *&data
		);

		/// <summary>
		/// 矩形スプライトの頂点データ・インデックスデータを生成する
		/// </summary>
		/// <param name="vertices">生成先の頂点データ</param>
		/// <param name="indices">生成先のインデックスデータ</param>
		/// <param name="left">左端座標</param>
		/// <param name="top">上端座標</param>
		/// <param name="right">右端座標</param>
		/// <param name="bottom">下端座標</param>
		void CreateSprite(
			std::vector<VertexData> &vertices,
			std::vector<uint32_t> &indices,
			float left,
			float top,
			float right,
			float bottom
		);

		/// <summary>
		/// 三角形メッシュの頂点データを生成する
		/// </summary>
		/// <param name="vertices">生成先の頂点データ</param>
		/// <param name="left">左頂点</param>
		/// <param name="top">上頂点</param>
		/// <param name="right">右頂点</param>
		void CreateTriangle(
			std::vector<VertexData> &vertices,
			Vector3 left, Vector3 top, Vector3 right
		);

		/// <summary>
		/// 四角形メッシュの頂点データを生成する
		/// </summary>
		/// <param name="vertices">生成先の頂点データ</param>
		/// <param name="center">中央座標</param>
		/// <param name="width">幅</param>
		/// <param name="depth">深さ</param>
		void CreatePlane(
			std::vector<VertexData> &vertices,
			Vector3 center, float width, float depth
		);

		/// <summary>
		/// 球体メッシュの頂点データとインデックスデータを生成する
		/// </summary>
		/// <param name="vertices">生成先の頂点配列</param>
		/// <param name="indices">生成先のインデックス配列</param>
		/// <param name="subdivision">球体の分割数</param>
		/// <remarks>
		/// 分割数が大きいほど滑らかな球体になるが、
		/// 頂点数・インデックス数も増加する。
		/// </remarks>
		void CreateSphere(
			std::vector<VertexData> &vertices,
			std::vector<uint32_t> &indices,
			uint32_t subdivision
		);
	};
}