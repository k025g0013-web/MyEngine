#pragma once
#include <d3d12.h>

class RasterizerState {
public:
    void Initialize();

    // getter
    const D3D12_RASTERIZER_DESC &GetDesc() const {return rasterizerDesc_;}

private:
    D3D12_RASTERIZER_DESC rasterizerDesc_{};
};