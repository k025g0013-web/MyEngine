#include "Material.h"

#include "Math/FunctionMatrix.h"
#include "Utils/ColorHelper.h"

void Material::Initialize(
    ID3D12Device *device,
    uint32_t color,
    bool enableLighting
) {
    // マテリアル情報を格納する定数バッファを生成する
    constantBuffer_.InitializeAsConstant(device, sizeof(MaterialData));

    // CPUから更新できるようにバッファをマッピングする
    materialData_ = static_cast<MaterialData *>(constantBuffer_.Map());

    // 初期カラーを設定する
    materialData_->color = UintToVector4(color);

    // ライティングの有効・無効を設定する
    materialData_->enableLighting = enableLighting ? 1 : 0;

    // 初期状態ではUV変換を行わないため単位行列を設定する
    materialData_->uvTransform = Math::MakeIdentity<Matrix4x4>();
}