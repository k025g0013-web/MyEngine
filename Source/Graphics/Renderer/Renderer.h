#pragma once
#include <vector>
#include "Graphics/Resource/MeshBuffer.h"
#include "RenderCore/TransformationMatrix.h"

namespace Kizuna {
	/// <summary>
	/// 描画用メッシュデータを生成する補助クラス
	/// </summary>
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
			const std::vector<VertexData> &vertices);
		
		/// <summary>
		/// インデックスデータからGPU用インデックスバッファを生成する
		/// </summary>
		/// <param name="device">Direct3Dデバイス</param>
		/// <param name="indexBuffer">生成先のインデックスバッファ</param>
		/// <param name="indices">インデックスデータ配列</param>
		void CreateIndexBuffer(
			ID3D12Device *device,
			MeshBuffer &indexBuffer,
			const std::vector<uint32_t> &indices);

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
	};
}