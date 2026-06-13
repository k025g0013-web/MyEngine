#pragma once

#include <d3d12.h>

#include "MeshBuffer.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

struct MaterialData {
    Vector4 color;
    int32_t enableLighting;
    float padding[3];
    Matrix4x4 uvTransform;
};

class Material {
public:
    void Initialize(
        ID3D12Device *device, uint32_t color, bool enableLighting
    );

    // getter
    MaterialData *GetMaterialData() const {return materialData_;}
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
        return constantBuffer_.GetGPUVirtualAddress();
    }

private:
    MeshBuffer constantBuffer_;

    MaterialData *materialData_ = nullptr;
};