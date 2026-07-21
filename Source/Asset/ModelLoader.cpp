#include "ModelLoader.h"

#include <fstream>
#include <sstream>
#include <cassert>

namespace Kizuna {
	MaterialSource ModelLoader::LoadMaterialTemplateFile(const std::string &modelDirectory, const std::string &fileName) {
		//=================================================================
		// 使用する変数
		//=================================================================

		// 読み込んだマテリアル情報
		MaterialSource materialData;

		// ファイルから読み込んだ1行
		std::string line;

		//=================================================================
		// mtlファイルを開く
		//=================================================================

		std::ifstream file(modelDirectory + "/" + fileName);

		// ファイルが開けなければ停止
		assert(file.is_open());

		//=================================================================
		// mtlファイルを解析
		//=================================================================

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
				materialData.textureFilePath =
					modelDirectory + "/" + textureFileName;
			}
		}

		// 構築したマテリアル情報を返す
		return materialData;
	}

	ModelData ModelLoader::LoadObjFile(const std::string &modelName) {

		//=================================================================
		// 読み込み対象のパス生成
		//=================================================================

		std::string modelDirectory = "Resources/Models/" + modelName;
		std::string objFilePath = modelDirectory + "/" + modelName + ".obj";

		//=================================================================
		// 使用する変数
		//=================================================================

		// 最終的に返すモデルデータ
		ModelData modelData;

		// 頂点位置
		std::vector<Vector4> positions;

		// 法線
		std::vector<Vector3> normals;

		// UV座標
		std::vector<Vector2> texcoords;

		// 読み込んだ1行
		std::string line;

		//=================================================================
		// objファイルを開く
		//=================================================================

		std::ifstream file(objFilePath);

		// ファイルが開けなければ停止
		assert(file.is_open());

		//=================================================================
		// objファイル解析
		//=================================================================

		while (std::getline(file, line)) {

			std::string identifier;
			std::istringstream s(line);

			// 行頭識別子を取得
			s >> identifier;

			//-------------------------------------------------------------
			// 頂点座標
			//-------------------------------------------------------------
			if (identifier == "v") {

				Vector4 position;
				s >> position.x >> position.y >> position.z;

				// 左手座標系へ変換
				position.x *= -1.0f;
				position.w = 1.0f;

				positions.push_back(position);
			}

			//-------------------------------------------------------------
			// UV座標
			//-------------------------------------------------------------
			else if (identifier == "vt") {

				Vector2 texcoord;
				s >> texcoord.x >> texcoord.y;

				// DirectX用に上下反転
				texcoord.y = 1.0f - texcoord.y;

				texcoords.push_back(texcoord);
			}

			//-------------------------------------------------------------
			// 法線
			//-------------------------------------------------------------
			else if (identifier == "vn") {

				Vector3 normal;
				s >> normal.x >> normal.y >> normal.z;

				// 左手座標系へ変換
				normal.x *= -1.0f;

				normals.push_back(normal);
			}

			//-------------------------------------------------------------
			// 面情報（三角形のみ対応）
			//-------------------------------------------------------------
			else if (identifier == "f") {

				// 三角形1枚分の頂点
				VertexData triangle[3];

				for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {

					std::string vertexDefinition;
					s >> vertexDefinition;

					// 「位置/UV/法線」を分割
					std::istringstream v(vertexDefinition);

					uint32_t elementIndices[3];

					for (int32_t element = 0; element < 3; ++element) {

						std::string index;

						// '/'区切りで取得
						std::getline(v, index, '/');

						elementIndices[element] = std::stoi(index);
					}

					// インデックスから各要素を取得
					Vector4 position = positions[elementIndices[0] - 1];
					Vector2 texcoord = texcoords[elementIndices[1] - 1];
					Vector3 normal = normals[elementIndices[2] - 1];

					VertexData vertex = { position, texcoord, normal };

					// 頂点リストへ追加
					modelData.vertices.push_back(vertex);

					triangle[faceVertex] = vertex;
				}

				// 頂点順を反転して表裏を合わせる
				modelData.vertices.push_back(triangle[2]);
				modelData.vertices.push_back(triangle[1]);
				modelData.vertices.push_back(triangle[0]);
			}

			//-------------------------------------------------------------
			// マテリアルファイル
			//-------------------------------------------------------------
			else if (identifier == "mtllib") {

				// mtlファイル名
				std::string materialFileName;
				s >> materialFileName;

				// マテリアル情報を読み込む
				modelData.material =
					LoadMaterialTemplateFile(modelDirectory, materialFileName);
			}
		}

		// 読み込んだモデルを返す
		return modelData;
	}
}