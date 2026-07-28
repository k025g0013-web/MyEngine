#include "PlaneObject.h"

namespace Kizuna {
	void PlaneObject::Create(
		ID3D12Device *device, uint32_t color, bool enableLighting) {
		// 頂点データを生成する
		renderer_.CreatePlane(vertices_, center_, width_, depth_);

		// メッシュを生成する
		mesh_.Create(device, vertices_);

		// 共通初期化
		InitializeMaterial(device, color, enableLighting);
	}
}