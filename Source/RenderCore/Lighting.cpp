#include "Lighting.h"

#include "Math/Functions.h"

void Lighting::Initialize(ID3D12Device *device) {
    // 並行光源用のResourceの作成
    constantBuffer_.InitializeAsConstant(device, sizeof(DirectionalLight));

    // データを書き込む
    lightingData_ = static_cast<DirectionalLight *>(constantBuffer_.Map());
    lightingData_->color = { 1.0f,1.0f,1.0f,1.0f };
    lightingData_->direction = { 0.0f,-1.0f,0.0f };
    lightingData_->intensity = 1.0f;
}

void Lighting::Update() {
    lightingData_->direction = Normalize(lightingData_->direction);
}

void Lighting::Bind(
    UINT rootParameterIndex,
    ID3D12GraphicsCommandList *commandList
) {
    commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, constantBuffer_.GetGPUVirtualAddress());
}