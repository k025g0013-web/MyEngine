#include "ModelObject.h"

#include <cassert>
#include <filesystem>

namespace Kizuna {

	void ModelObject::Create(
		ID3D12Device *device,
		ID3D12GraphicsCommandList *commandList,
		uint32_t color,
		bool enableLighting) {

		// OBJを読み込む
		modelData_ = ModelLoader::LoadObjFile(fileName_);

		// OBJからメッシュを生成
		mesh_.Create(
			device,
			modelData_.vertices);

		// 外部テクスチャが指定されていない場合
		// OBJと同じ名前のPNGを自動的に読み込む
		if (!useExternalTexture_) {

			assert(texture_ != nullptr);

			std::filesystem::path texturePath =
				std::filesystem::path("resources/Models")
				/ fileName_
				/ (fileName_ + ".png");

			textureData_ =
				texture_->LoadTexture(
					commandList,
					texturePath.string());
		}

		// マテリアル初期化
		InitializeMaterial(
			device,
			color,
			enableLighting);
	}

	void ModelObject::Draw(
		ID3D12GraphicsCommandList *commandList) {

		// マテリアル
		commandList->SetGraphicsRootConstantBufferView(
			0,
			material_.GetGPUVirtualAddress());

		// WVP
		commandList->SetGraphicsRootConstantBufferView(
			1,
			transformationMatrixBuffer_.GetGPUVirtualAddress());

		// テクスチャ
		commandList->SetGraphicsRootDescriptorTable(
			2,
			textureData_.gpuHandle);

		// メッシュ
		mesh_.Bind(commandList);
	}
}