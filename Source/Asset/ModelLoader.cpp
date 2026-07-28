#include "ModelLoader.h"

#include <fstream>
#include <sstream>
#include <cassert>

namespace Kizuna {
	MaterialSource ModelLoader::LoadMaterialTemplateFile(const std::string &directoryPath, const std::string &fileName) {
		// 中で必要となる変数の宣言
		MaterialSource materialData;	// 構築するMaterialData
		std::string line;	// ファイルから読んだ1行を格納するもの

		// ファイルを開く
		std::ifstream file(directoryPath + "/" + fileName);

		// mtlファイルが開けなかった場合はデフォルトマテリアルを返す
		if (!file.is_open()) {
			materialData.textureFilePath = "";
			return materialData;
		}

		// 実際にファイルを読み、MaterialDataを構築していく
		while (std::getline(file, line)) {
			// 行の識別子
			std::string identifier;

			std::istringstream s(line);
			s >> identifier;

			// テクスチャファイル名
			if (identifier == "map_Kd") {
				std::string textureFileName;
				s >> textureFileName;

				// モデルフォルダと結合してフルパスを作成
				materialData.textureFilePath = directoryPath + "/" + textureFileName;
			}
		}

		// 構築したマテリアル情報を返す
		return materialData;
	}

	ModelData ModelLoader::LoadObjFile(const std::string &modelName) {
		// 読み込み対象のパス生成
		std::string modelDirectory = "Resources/Models/" + modelName;
		std::string objFilePath = modelDirectory + "/" + modelName + ".obj";

		// 中で必要となる変数の宣言
		ModelData modelData;	// 最終的に返すモデルデータ
		std::vector<Vector4> positions;	// 頂点位置
		std::vector<Vector3> normals;	// 法線
		std::vector<Vector2> texcoords;	// テクスチャ座標
		std::string line;	// ファイルから読んだ1行を格納するもの

		// ファイルを開く
		std::ifstream file(objFilePath);

		// ファイルが開けなければ停止
		assert(file.is_open());

		// 実際にファイルを読み、ModelDataを構築していく
		while (std::getline(file, line)) {
			std::string identifier;
			std::istringstream s(line);
			// 行頭識別子を取得
			s >> identifier;

			if (identifier == "v") {		// 位置 
				Vector4 position;
				s >> position.x >> position.y >> position.z;

				// 左手座標系へ変換
				position.x *= -1.0f;
				position.w = 1.0f;

				positions.push_back(position);

			} else if (identifier == "vt") {	// テクスチャ座標
				Vector2 texcoord{};
				s >> texcoord.x >> texcoord.y;

				// DirectX用に上下反転（0.0 〜 1.0 の範囲内のみ反転）
				texcoord.y = 1.0f - texcoord.y;

				texcoords.push_back(texcoord);

			} else if (identifier == "vn") {	// 法線
				Vector3 normal;
				s >> normal.x >> normal.y >> normal.z;

				// 左手座標系へ変換
				normal.x *= -1.0f;

				normals.push_back(normal);

			} else if (identifier == "f") {	// 面
				std::vector<VertexData> faceVertices;
				std::string vertexDefinition;

				while (s >> vertexDefinition) {
					std::istringstream v(vertexDefinition);
					std::string indexStr;

					int32_t posIndex = 0;
					int32_t texIndex = 0;
					int32_t normIndex = 0;

					if (std::getline(v, indexStr, '/') && !indexStr.empty()) posIndex = std::stoi(indexStr);
					if (std::getline(v, indexStr, '/') && !indexStr.empty()) texIndex = std::stoi(indexStr);
					if (std::getline(v, indexStr, '/') && !indexStr.empty()) normIndex = std::stoi(indexStr);

					// インデックスから実データを参照（未定義の場合はデフォルト値）
					Vector4 pos = (posIndex > 0 && posIndex <= positions.size()) ? positions[posIndex - 1] : Vector4{ 0,0,0,1 };
					Vector2 uv = (texIndex > 0 && texIndex <= texcoords.size()) ? texcoords[texIndex - 1] : Vector2{ 0,0 };
					Vector3 norm = (normIndex > 0 && normIndex <= normals.size()) ? normals[normIndex - 1] : Vector3{ 0,1,0 };

					faceVertices.push_back({ pos, uv, norm });
				}

				// 三角形化して時計回り（左手系）に格納
				if (faceVertices.size() >= 3) {
					for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
						// 表面を向けるため反転順序で追加
						modelData.vertices.push_back(faceVertices[0]);
						modelData.vertices.push_back(faceVertices[i + 1]);
						modelData.vertices.push_back(faceVertices[i]);
					}
				}
			} else if (identifier == "mtllib") {	// Material読み込み
				// materialTemplateLibraryファイルの名前を取得する
				std::string  materialFileName;
				s >> materialFileName;

				// 基本的にobjファイルと同一階層にmtlは存在させるのでディレクトリ名とファイル名を探す
				modelData.material = LoadMaterialTemplateFile(modelDirectory, materialFileName);
			}
		}

		// modelDataを返す
		return modelData;
	}
}