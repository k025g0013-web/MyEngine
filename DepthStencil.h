#pragma once

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

#include "DescriptorHeap.h"

class DepthStencil {
public:
    void Initialize(ID3D12Device *device, uint32_t width, uint32_t height);

    // getter
    ID3D12Resource *GetResource() const {return depthStencilResource_.Get();}
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const {return dsvHeap_.GetCPUDescriptorHandle(0);}

private:
    void CreateDepthStencilResource(ID3D12Device *device, uint32_t width, uint32_t height);

    void CreateDepthStencilView(ID3D12Device *device);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

    DescriptorHeap dsvHeap_;

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};
};