#include "ModelObject.h"

namespace Kizuna {
	void ModelObject::Create(
		ID3D12Device *device, uint32_t color, bool enableLighting) {
		// objモデルを読み込む
		modelData_ = ModelLoader::LoadObjFile(fileName_);

		// メッシュ生成
		mesh_.Create(device, modelData_.vertices);

		// 共通初期化
		InitializeMaterial(device, color, enableLighting);
	}
}