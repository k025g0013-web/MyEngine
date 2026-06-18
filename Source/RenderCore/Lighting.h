#pragma once

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"

struct DirectionalLight {
    Vector4 color;
    Vector3 direction;
    float intensity;
    int lightType;
    float padding[3];
};

class Lighting {
public:
    enum class LightingType {
        None,
        Lambert,
        Half_Lambert,
    };
public:
    void Initialize(ID3D12Device *device);

    void Update();

    void Bind(UINT rootParameterIndex, ID3D12GraphicsCommandList *commandList);

    void SetLightType(LightingType type);

    DirectionalLight *GetLightingData() { return lightingData_; }
    LightingType GetLightType() const { return currentLightType_; }

private:
    MeshBuffer constantBuffer_;
    DirectionalLight *lightingData_ = nullptr;

    LightingType currentLightType_{};
};