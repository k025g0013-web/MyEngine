#pragma once
#include <d3d12.h>

class BlendState {
public:
    void Initialize();

    // getter
    const D3D12_BLEND_DESC &GetDesc() const {return blendDesc_;}

private:
    D3D12_BLEND_DESC blendDesc_{};
};