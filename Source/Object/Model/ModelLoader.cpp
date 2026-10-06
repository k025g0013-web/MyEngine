#include "ModelLoader.h"

#include <cassert>
#include <fstream>
#include <sstream>

namespace Kizuna {

    ModelData ModelLoader::LoadObjFile(
        const std::string &modelName) {

        // 読み込み対象のパスを生成
        const std::string modelDirectory =
            "Resources/Models/" + modelName;

        const std::string objFilePath =
            modelDirectory + "/" + modelName + ".obj";

        // 最終的に返すモデルデータ
        ModelData modelData;

        // OBJから読み込む一時データ
        std::vector<Vector4> positions;
        std::vector<Vector3> normals;
        std::vector<Vector2> texcoords;

        // ファイルから読み込んだ1行
        std::string line;

        // OBJファイルを開く
        std::ifstream file(objFilePath);

        // ファイルが開けなければ停止
        assert(file.is_open());

        // OBJファイルを読み込む
        while (std::getline(file, line)) {

            std::string identifier;
            std::istringstream s(line);

            // 行頭識別子を取得
            s >> identifier;

            if (identifier == "v") {

                // 頂点位置
                Vector4 position;

                s >> position.x
                    >> position.y
                    >> position.z;

                // 左手座標系へ変換
                position.x *= -1.0f;
                position.w = 1.0f;

                positions.push_back(position);

            } else if (identifier == "vt") {

                // テクスチャ座標
                Vector2 texcoord{};

                s >> texcoord.x
                    >> texcoord.y;

                // DirectX用に上下反転
                texcoord.y = 1.0f - texcoord.y;

                texcoords.push_back(texcoord);

            } else if (identifier == "vn") {

                // 法線
                Vector3 normal;

                s >> normal.x
                    >> normal.y
                    >> normal.z;

                // 左手座標系へ変換
                normal.x *= -1.0f;

                normals.push_back(normal);

            } else if (identifier == "f") {

                // 面の頂点
                std::vector<VertexData> faceVertices;

                std::string vertexDefinition;

                while (s >> vertexDefinition) {

                    std::istringstream v(vertexDefinition);
                    std::string indexStr;

                    int32_t posIndex = 0;
                    int32_t texIndex = 0;
                    int32_t normIndex = 0;

                    // 頂点位置インデックス
                    if (
                        std::getline(v, indexStr, '/')
                        && !indexStr.empty()) {

                        posIndex = std::stoi(indexStr);
                    }

                    // テクスチャ座標インデックス
                    if (
                        std::getline(v, indexStr, '/')
                        && !indexStr.empty()) {

                        texIndex = std::stoi(indexStr);
                    }

                    // 法線インデックス
                    if (
                        std::getline(v, indexStr, '/')
                        && !indexStr.empty()) {

                        normIndex = std::stoi(indexStr);
                    }

                    // 頂点位置
                    Vector4 pos =
                        (posIndex > 0 &&
                            posIndex <= positions.size())
                        ? positions[posIndex - 1]
                        : Vector4{ 0, 0, 0, 1 };

                    // テクスチャ座標
                    Vector2 uv =
                        (texIndex > 0 &&
                            texIndex <= texcoords.size())
                        ? texcoords[texIndex - 1]
                        : Vector2{ 0, 0 };

                    // 法線
                    Vector3 norm =
                        (normIndex > 0 &&
                            normIndex <= normals.size())
                        ? normals[normIndex - 1]
                        : Vector3{ 0, 1, 0 };

                    faceVertices.push_back({
                        pos,
                        uv,
                        norm
                        });
                }

                // 三角形化
                if (faceVertices.size() >= 3) {

                    for (
                        size_t i = 1;
                        i + 1 < faceVertices.size();
                        ++i) {

                        // 時計回りになるように追加
                        modelData.vertices.push_back(
                            faceVertices[0]);

                        modelData.vertices.push_back(
                            faceVertices[i + 1]);

                        modelData.vertices.push_back(
                            faceVertices[i]);
                    }
                }
            }
        }

        return modelData;
    }
}