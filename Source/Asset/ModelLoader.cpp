#include "ModelLoader.h"

#include <fstream>
#include <sstream>
#include <cassert>

MaterialSource ModelLoader::LoadMaterialTemplateFile(const std::string &modelDirectory, const std::string &fileName) {
	// 中で必要となる変数の宣言
	MaterialSource materialData;	// 構築するMaterialData
	std::string line;	// ファイルから読んだ1行を格納するもの

	// ファイルを開く
	std::ifstream file(modelDirectory + "/" + fileName);
	assert(file.is_open());	// とりあえず開けなかったら止める

	// 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFileName;
			s >> textureFileName;

			// 連結してファイルパスにする
			materialData.textureFilePath = modelDirectory + "/" + textureFileName;
		}
	}

	// materialDataを返す
	return materialData;
}

ModelData ModelLoader::LoadObjFile(const std::string &modelName) {
	std::string modelDirectory = "Resources/Models/" + modelName;
	std::string objFilePath = modelDirectory + "/" + modelName + ".obj";

	// 中で必要となる変数の宣言

	ModelData modelData;	// 構築するModelData
	std::vector<Vector4> positions;	// 位置
	std::vector<Vector3> normals;	// 法線
	std::vector<Vector2> texcoords;	// テクスチャ座標
	std::string line;	// ファイルから読んだ1行を格納するもの

	// 自動生成したパスでファイルを開く
	std::ifstream file(objFilePath);
	assert(file.is_open());	// とりあえず開けなかったら止める

	// 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	// 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {		// 位置 
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);

		} else if (identifier == "vt") {	// テクスチャ座標
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);

		} else if (identifier == "vn") {	// 法線
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);

		} else if (identifier == "f") {	// 面
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');	// 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

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
