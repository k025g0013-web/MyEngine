#pragma once

#pragma comment(lib, "d3d12.lib")

#include <wrl.h>
#include <d3d12.h>

class Logger;

class RootSignature {
public:
    //  RootSignatureの用途を定義
    enum class Type {
        Skinny3D,   // ライトありの3D用
        Skinny2D,   // ライトなしの2D用
    };

public:
    void Initialize(ID3D12Device *device, Logger *logger, Type type);

    // getter
    ID3D12RootSignature *GetRootSignature() const {return rootSignature_.Get();}

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};