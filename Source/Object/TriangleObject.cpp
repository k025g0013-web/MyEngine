#include "TriangleObject.h"

namespace Kizuna {
	void TriangleObject::Create(
		ID3D12Device *device, ID3D12GraphicsCommandList *commandList,
		uint32_t color, bool enableLighting) {

		// commandListはプリミティブでは使用しない
		(void)commandList;

		// 頂点データを生成する
		meshGenerator_.CreateTriangle(vertices_, wigth_, height_, right_);

		// メッシュを生成する
		mesh_.Create(device, vertices_);

		// 共通初期化
		InitializeMaterial(device, color, enableLighting);
	}
}