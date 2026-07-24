#include "TriangleObject.h"

namespace Kizuna {
	void TriangleObject::Create(
		ID3D12Device *device, uint32_t color, bool enableLighting) {
		// 頂点データを生成する
		renderer_.CreateTriangle(vertices_, left_, top_, right_);

		// メッシュを生成する
		mesh_.Create(device, vertices_);

		// 共通初期化
		InitializeMaterial(device, color, enableLighting);
	}
}