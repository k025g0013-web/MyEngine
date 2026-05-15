#include "RenderTargetViews.h"

#include "SwapChain.h"
#include "DescriptorHeap.h"

#include <cassert>

// RTVの設定
void RenderTargetViews::Initialize(
    ID3D12Device* device, SwapChain* swapChain, DescriptorHeap* descriptorHeap
) {
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// 出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// 2dテクスチャとして書き込む

	// 2つのディスクリプタハンドルを得る 
    for (uint32_t i = 0; i < kRenderTargetCount; ++i) {
		// ディスクリプタの先頭を取得する
        rtvHandles_[i] = descriptorHeap->GetCPUDescriptorHandle(i);

		// 2つ目のディスクリプタハンドルを得る
        device->CreateRenderTargetView(swapChain->GetBackBuffer(i), &rtvDesc_, rtvHandles_[i]);
    }
}