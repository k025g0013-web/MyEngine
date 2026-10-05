#pragma once

#include <string>
#include <utility>

#include "Common/Object3D.h"
#include "Asset/ModelLoader.h"
#include "Graphics/Resource/Texture.h"

namespace Kizuna {

	class ModelObject : public Object3D {
	public:

		/// <summary>
		/// OBJと同名のPNGを自動的に読み込む
		/// </summary>
		ModelObject(
			std::string fileName,
			Texture *texture)
			: fileName_(std::move(fileName)),
			texture_(texture) {}

		/// <summary>
		/// 外部から指定したテクスチャを使用する
		/// </summary>
		ModelObject(
			std::string fileName,
			Texture *texture,
			const TextureData &textureData)
			: fileName_(std::move(fileName)),
			texture_(texture),
			textureData_(textureData),
			useExternalTexture_(true) {}

		void Create(
			ID3D12Device *device,
			ID3D12GraphicsCommandList *commandList,
			uint32_t color,
			bool enableLighting) override;

		void Draw(
			ID3D12GraphicsCommandList *commandList) override;

	private:

		// モデルファイル名
		std::string fileName_;

		// モデルデータ
		ModelData modelData_;

		// テクスチャ管理クラス
		Texture *texture_ = nullptr;

		// モデルに使用するテクスチャ
		TextureData textureData_{};

		// 外部からテクスチャを指定したか
		bool useExternalTexture_ = false;
	};
}