#include "ThroughWallObject.h"

#include "Object/Common/Object3D.h"

namespace Kizuna {

    void ThroughWallObject::Draw(
        ID3D12GraphicsCommandList *commandList) {

        ThroughWallMaterialData *material =
            object_->GetThroughWallMaterial().GetMaterialData();

        // 壁越し描画の色を設定
        material->color = color_;

        // 壁越し描画のスタイルを設定
        material->style = static_cast<int32_t>(style_);

        // 対象オブジェクトを壁越し描画
        object_->DrawThroughWall(commandList);
    }
}