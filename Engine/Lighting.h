#pragma once

#include "ConstantBuffer.h"
#include "Vector3.h"
#include "Vector4.h"

struct DirectionalLight {
    Vector4 color;
    Vector3 direction;
    float intensity;
};

class Lighting {
public:
    void Initialize(ID3D12Device *device);

    void Update();

    void Bind(UINT rootParameterIndex, ID3D12GraphicsCommandList *commandList);

    DirectionalLight *GetLightingData() { return lightingData_; }

private:

    ConstantBuffer buffer_;
    DirectionalLight *lightingData_ = nullptr;
};