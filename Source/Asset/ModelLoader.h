#pragma once

#include <string>
#include <vector>

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"

namespace Kizuna {
	/// <summary>
	/// マテリアルファイルから取得した情報を保持する構造体
	/// </summary>
	struct MaterialSource {
		// 使用するテクスチャファイルへのパス
		std::string textureFilePath;
	};

	/// <summary>
	/// モデルデータを保持する構造体
	/// </summary>
	/// <remarks>
	/// 頂点情報とマテリアル情報をまとめて保持する。
	/// </remarks>
	struct ModelData {
		// モデルの頂点データ
		std::vector<VertexData> vertices;

		// モデルに対応するマテリアル情報
		MaterialSource material;
	};

	/// <summary>
	/// objモデル・mtlファイルを読み込むローダークラス
	/// </summary>
	/// <remarks>
	/// Wavefront(.obj)形式のモデルを読み込み、
	/// 描画用のModelDataを生成する。
	/// </remarks>
	class ModelLoader {
	public:

		/// <summary>
		/// mtlファイルを読み込みマテリアル情報を取得する
		/// </summary>
		static MaterialSource LoadMaterialTemplateFile(
			const std::string &directoryPath,
			const std::string &fileName
		);

		/// <summary>
		/// objファイルを読み込みモデルデータを生成する
		/// </summary>
		static ModelData LoadObjFile(
			const std::string &modelName
		);
	};
}