#include "Renderer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Math/FunctionMatrix.h"

namespace Kizuna {
	void Renderer::CreateVertexBuffer(
		ID3D12Device *device,
		MeshBuffer &vertexBuffer,
		const std::vector<VertexData> &vertices
	) {
		// GPU上に頂点バッファを生成する
		vertexBuffer.InitializeAsVertex(device, sizeof(VertexData) * vertices.size(), sizeof(VertexData));

		// CPUから書き込むためにバッファをマッピングする
		VertexData *vertexData = static_cast<VertexData *>(vertexBuffer.Map());

		// CPU側で生成した頂点データをGPUリソースへコピーする
		std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
	}

	void Renderer::CreateIndexBuffer(
		ID3D12Device *device,
		MeshBuffer &indexBuffer,
		const std::vector<uint32_t> &indices
	) {
		// GPU上にインデックスバッファを生成する
		indexBuffer.InitializeAsIndex(device, sizeof(uint32_t) * indices.size());

		// CPUから書き込むためにバッファをマッピングする
		uint32_t *indexData = static_cast<uint32_t *>(indexBuffer.Map());

		// CPU側で生成したインデックスデータをGPUリソースへコピーする
		std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
	}

	void Renderer::CreateTransformationMatrixBuffer(
		ID3D12Device *device,
		MeshBuffer &constantBuffer,
		TransformationMatrix *&data
	) {
		// ワールド行列・WVP行列を格納する定数バッファを生成する
		constantBuffer.InitializeAsConstant(device, sizeof(TransformationMatrix));

		// CPUから更新できるようにバッファをマッピングする
		data = static_cast<TransformationMatrix *>(constantBuffer.Map());

		// 初期状態では変換を行わないよう単位行列で初期化する
		data->WVP = Math::MakeIdentity<Matrix4x4>();
		data->World = Math::MakeIdentity<Matrix4x4>();
	}


}