#pragma once
#include <d3d12.h>

class DepthStencilState {
public:
    void Initialize();

    const D3D12_DEPTH_STENCIL_DESC &GetDesc() const {
        return depthStencilDesc_;
    }

private:
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
};