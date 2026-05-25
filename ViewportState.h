#pragma once
#include <d3d12.h>

class ViewportState {
public:
    void Initialize(float width, float height);

    void SetCommand(
        ID3D12GraphicsCommandList *commandList
    );

private:
    D3D12_VIEWPORT viewport_;
    D3D12_RECT scissorRect_;
};