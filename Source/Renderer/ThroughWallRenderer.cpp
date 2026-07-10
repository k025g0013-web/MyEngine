#include "ThroughWallRenderer.h"
#include "Object/Object3D.h"
#include "Utils/ColorHelper.h"

#include <iostream>

void ThroughWallRenderer::AddObject(Object3D *object, uint32_t color) {

    ThroughWallObject data;

    // 壁越し描画を行うオブジェクトを登録する
    data.object = object;

    // シェーダーで扱えるようRGBA(0～1)へ変換して保持する
    data.color = UintToVector4(color);

    objects_.push_back(data);
}

void ThroughWallRenderer::Draw(
    ID3D12GraphicsCommandList *commandList,
    TextureData &texture) {

    // 登録済みの全オブジェクトを壁越し描画する
    for (auto &data : objects_) {

        MaterialData *material =
            data.object->GetThroughWallMaterial().GetMaterialData();

        // 壁越し描画専用の色をマテリアルへ設定する
        // 通常描画用マテリアルとは分離することで、
        // 通常描画時の色へ影響を与えないようにしている。
        material->color = data.color;

        // 壁越し描画専用パイプラインで描画する
        data.object->DrawThroughWall(commandList, texture);
    }
}