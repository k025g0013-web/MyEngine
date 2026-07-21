#include "Fence.h"

namespace Kizuna {
Fence::~Fence() {
    if (fenceEvent_) {
        CloseHandle(fenceEvent_);
        fenceEvent_ = nullptr;
    }
}

// 初期化
void Fence::Initialize(ID3D12Device* device) {
    // Fenceリソース生成
    // 初期値を指定し、GPU処理完了位置を管理できるようにする
    HRESULT hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));
    (void)hr;

    // GPU完了通知を受け取るためのイベントを生成する
    // Fence値が指定値へ到達した際にOSイベントを通知するために使用する
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(fenceEvent_ != nullptr);
}

// GPU待機
void Fence::Wait(ID3D12CommandQueue* commandQueue) {
    // 待機対象となるFence値を更新する
    // 毎回異なる値を使用することで、フレームごとの同期位置を管理する
    fenceValue_++;
    // GPUへSignalを送信する
    // GPUがこの位置まで処理を完了した時点でFence値が更新される
    commandQueue->Signal(fence_.Get(), fenceValue_);

    // GPU側の処理が完了しているか確認する
    if (fence_->GetCompletedValue() < fenceValue_) {
        // まだGPUがSignal位置まで到達していない場合、
        // 指定Fence値に到達した時点でイベントを通知するよう設定する
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        // GPU完了までCPU側を待機させる
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}
}