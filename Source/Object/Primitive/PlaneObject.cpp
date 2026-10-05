#include "PlaneObject.h"

namespace Kizuna {
	void PlaneObject::Create(
		ID3D12Device *device, ID3D12GraphicsCommandList *commandList,
		uint32_t color, bool enableLighting) {
		// commandListはプリミティブでは使用しない
		(void)commandList;

		std::vector<VertexData> vertices;

		meshGenerator_.CreatePlane(
			vertices,
			center_,
			width_,
			depth_);

		// メッシュを生成する
		mesh_.Create(device, vertices);

		// 共通初期化
		InitializeMaterial(device, color, enableLighting);
	}
}