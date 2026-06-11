#pragma once
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

class Logger;

class RootSignature {
public:
    void Initialize(ID3D12Device *device, Logger *logger);

    // getter
    ID3D12RootSignature *GetRootSignature() const {return rootSignature_.Get();}

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};