#pragma once

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"

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

    MeshBuffer constantBuffer_;
    DirectionalLight *lightingData_ = nullptr;
};