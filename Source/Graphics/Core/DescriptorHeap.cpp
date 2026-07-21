#include "DescriptorHeap.h"

#include <cassert>

namespace Kizuna {
	// DescriptorHeap生成
	void DescriptorHeap::Initialize(
		ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible
	) {
		// DescriptorHeapの設定を作成する
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		// RTV、DSV、CBV_SRV_UAVなど用途に応じたHeap種類を設定する
		descriptorHeapDesc.Type = heapType;
		// 格納するDescriptor数を設定する
		descriptorHeapDesc.NumDescriptors = numDescriptors;
		// ShaderVisibleの場合、GPUから参照可能なDescriptorHeapを生成する
		// TextureやConstantBufferなどをShaderで使用する場合に必要
		descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		// DescriptorHeap生成
		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
		assert(SUCCEEDED(hr));
		(void)hr;

		// Descriptor間のアドレス差分を取得する
		// DirectX12ではDescriptorは連続したメモリ領域に配置されるため、
		// このサイズを利用して任意のDescriptor位置を計算する
		descriptorSize_ = device->GetDescriptorHandleIncrementSize(heapType);
	}

	// getter
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(uint32_t index) const {
		// DescriptorHeapの先頭アドレスを取得
		D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();

		// index番目のDescriptor位置まで移動する
		// Descriptorは固定サイズで連続配置されているため、
		// サイズ×番号で目的位置を求められる
		handle.ptr += descriptorSize_ * index;
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(uint32_t index) const {
		// DescriptorHeapのGPU側先頭アドレスを取得
		D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
		// 指定されたDescriptor位置までoffsetを加算する
		handle.ptr += descriptorSize_ * index;
		return handle;
	}
}