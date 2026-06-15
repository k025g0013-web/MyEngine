#pragma once
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <d3d12.h>
#include <dxgi1_6.h>

class Logger;

class DirectXDevice {
public:
    void Initialize(Logger* logger);

    // getter
    ID3D12Device* GetDevice() const { return device_.Get(); }
    IDXGIFactory7* GetDXGIFactory() const { return dxgiFactory_.Get(); }
    IDXGIAdapter4* GetAdapter() const { return useAdapter_.Get(); }

private:
    // 使用するアダプタ(GPU)の選択
    void SelectAdapter();
    
    // Deviceの生成
    void CreateDevice();
    
    // デバッグレイヤー生成
    void SetupDebugLayer();

private:
    Logger* logger_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
};