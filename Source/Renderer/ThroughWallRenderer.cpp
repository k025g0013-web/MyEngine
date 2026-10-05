#include "ThroughWallRenderer.h"
#include "Object/Common/Object3D.h"
#include "Utils/ColorHelper.h"

#include <iostream>
#include <algorithm>

namespace Kizuna {
    void ThroughWallRenderer::AddObject(Object3D *object, uint32_t color, Style style) {
        ThroughWallObject data{};

        // 壁越し描画を行うオブジェクトを登録する
        data.object = object;

        // シェーダーで扱えるようRGBA(0～1)へ変換して保持する
        data.color = UintToVector4(color);

        // 指定された壁越し描画時の描画スタイルを登録する
        data.style = style;

        objects_.push_back(data);
    }

    void ThroughWallRenderer::Draw(
        ID3D12GraphicsCommandList *commandList) {
        // 登録済みの全オブジェクトを壁越し描画する
        for (auto &data : objects_) {

            ThroughWallMaterialData *material =
                data.object->GetThroughWallMaterial().GetMaterialData();

            // 壁越し描画専用の色をマテリアルへ設定する
            // 通常描画用マテリアルとは分離することで、
            // 通常描画時の色へ影響を与えないようにしている。
            material->color = data.color;

            // 壁越し描画専用の描画スタイルをマテリアルへ設定する。
            // 通常描画用マテリアルとは分離することで、
            // 壁越し描画専用の表現を個別に制御できるようにしている。
            material->style = static_cast<int32_t>(data.style);

            // 壁越し描画専用パイプラインで描画する
            data.object->DrawThroughWall(commandList);
        }
    }

    void ThroughWallRenderer::RemoveObject(Object3D *object) {

        objects_.erase(
            std::remove_if(
                objects_.begin(),
                objects_.end(),
                [object](const ThroughWallObject &data) {
                    return data.object == object;
                }
            ),
            objects_.end()
        );
    }
}