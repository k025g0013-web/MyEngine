#include "Lighting.h"

#include "Math/FunctionVector.h"

namespace Kizuna {
    void Lighting::Initialize(ID3D12Device *device) {

        // ライト情報を格納する定数バッファを生成する
        constantBuffer_.InitializeAsConstant(device, sizeof(DirectionalLight));

        // CPUから更新できるようにバッファをマッピングする
        lightingData_ = static_cast<DirectionalLight *>(constantBuffer_.Map());

        // ライトの初期値を設定する
        lightingData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
        lightingData_->direction = { 0.0f, -1.0f, 0.0f };
        lightingData_->intensity = 1.0f;

        // 初期ライティング方式を設定する
        currentLightType_ = LightingType::None;
        SetLightType(LightingType::Half_Lambert);

        // ConstantBufferのアライメント調整用
        lightingData_->padding[0] = 0.0f;
        lightingData_->padding[1] = 0.0f;
        lightingData_->padding[2] = 0.0f;
    }

    void Lighting::Update() {

        // ライティング計算が正しく行えるよう光の方向を正規化する
        lightingData_->direction = Math::Normalize(lightingData_->direction);
    }

    void Lighting::Bind(
        UINT rootParameterIndex,
        ID3D12GraphicsCommandList *commandList
    ) {
        // ライト用定数バッファをシェーダへ設定する
        commandList->SetGraphicsRootConstantBufferView(
            rootParameterIndex,
            constantBuffer_.GetGPUVirtualAddress()
        );
    }

    void Lighting::SetLightType(LightingType type) {

        // ライト情報が生成済みの場合のみ設定を反映する
        if (lightingData_) {
            currentLightType_ = type;
            lightingData_->lightType = static_cast<int>(type);
        }
    }
}