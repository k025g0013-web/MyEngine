#include "Fence.h"

Fence::~Fence() {
    if (fenceEvent_) {
        CloseHandle(fenceEvent_);
        fenceEvent_ = nullptr;
    }
}

// 初期化
void Fence::Initialize(ID3D12Device* device) {
    HRESULT hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));

    // イベント生成
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(fenceEvent_ != nullptr);
}

// GPU待機
void Fence::Wait(ID3D12CommandQueue* commandQueue) {
    // Fence値を更新
    fenceValue_++;
    // GPUがここまでたどり着いたときに、Fenceの値に代入するようにSignalを送る
    commandQueue->Signal(fence_.Get(), fenceValue_);

    // GPU完了待ち
    if (fence_->GetCompletedValue() < fenceValue_) {
        // 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        // イベント待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}