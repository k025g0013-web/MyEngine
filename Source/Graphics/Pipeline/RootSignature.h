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
    // 標準的な3D描画用
    void CreateSkinny3D(ID3D12Device *device, Logger *logger);

    // Lighting無し2D用
    void CreateSkinny2D(ID3D12Device *device, Logger *logger);

    void CreateThroughWall3D(ID3D12Device *device, Logger *logger);

    // getter
    ID3D12RootSignature *GetRootSignature() const {return rootSignature_.Get();}

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};