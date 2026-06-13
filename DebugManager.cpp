#include "DebugManager.h"
#include <cassert>
#include <objbase.h>

DebugManager::DebugManager() {
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) {
        isComInitialized_ = true;
    }
    (void)hr;
}

DebugManager::~DebugManager() {
    if (isComInitialized_) {
        CoUninitialize();
    }
}

void DebugManager::EnableDebugLayer() {
#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバッグレイヤーを有効化
        debugController->EnableDebugLayer();
        // GPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif
}