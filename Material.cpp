#include "Material.h"

#include "MathFunctions.h"
#include "ColorUtility.h"

void Material::Initialize(
    ID3D12Device *device, uint32_t color, bool enableLighting
) {
    // リソース生成
    constantBuffer_.Initialize(device, sizeof(MaterialData));

    // データを書き込む
    materialData_ = static_cast<MaterialData *>(constantBuffer_.Map());
    materialData_->color = UintToVector4(color);    // 色データを書き込む
    materialData_->enableLighting = enableLighting ? 1 : 0; // lightingの有無

    // 単位行列を書き込んでおく
    materialData_->uvTransform = MakeIdentity4x4();
}