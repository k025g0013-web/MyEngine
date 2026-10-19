#include "DescriptorHeap.h"

#include <cassert>

namespace Kizuna {

    void DescriptorHeap::Initialize(
        ID3D12Device *device,
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible) {

        assert(device);
        assert(numDescriptors > 1);

        // DescriptorHeapの設定
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
        descriptorHeapDesc.Type = heapType;
        descriptorHeapDesc.NumDescriptors = numDescriptors;
        descriptorHeapDesc.Flags =
            shaderVisible
            ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
            : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        // DescriptorHeap生成
        HRESULT hr = device->CreateDescriptorHeap(
            &descriptorHeapDesc,
            IID_PPV_ARGS(&descriptorHeap_)
        );

        assert(SUCCEEDED(hr));
        (void)hr;

        // Descriptorサイズを取得
        descriptorSize_ =
            device->GetDescriptorHandleIncrementSize(heapType);

        // Descriptor数を保存
        descriptorCount_ = numDescriptors;

        // インデックス0は予約する既存仕様を維持
        nextDescriptorIndex_ = 1;

        // 解放済みリストを初期化
        freeDescriptorIndices_.clear();

        // 全インデックスを未割り当てとして初期化
        isDescriptorAllocated_.assign(
            descriptorCount_,
            false
        );

        // インデックス0は予約済みとして扱う
        isDescriptorAllocated_[0] = true;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE
        DescriptorHeap::GetCPUDescriptorHandle(uint32_t index) const {

        assert(index < descriptorCount_);

        D3D12_CPU_DESCRIPTOR_HANDLE handle =
            descriptorHeap_->GetCPUDescriptorHandleForHeapStart();

        handle.ptr +=
            static_cast<SIZE_T>(descriptorSize_) * index;

        return handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE
        DescriptorHeap::GetGPUDescriptorHandle(uint32_t index) const {

        assert(index < descriptorCount_);

        D3D12_GPU_DESCRIPTOR_HANDLE handle =
            descriptorHeap_->GetGPUDescriptorHandleForHeapStart();

        handle.ptr +=
            static_cast<UINT64>(descriptorSize_) * index;

        return handle;
    }

    uint32_t DescriptorHeap::AllocateDescriptor() {

        // 解放済みDescriptorがあれば再利用
        if (!freeDescriptorIndices_.empty()) {

            const uint32_t index =
                freeDescriptorIndices_.back();

            freeDescriptorIndices_.pop_back();

            assert(index > 0);
            assert(index < descriptorCount_);
            assert(!isDescriptorAllocated_[index]);

            isDescriptorAllocated_[index] = true;

            return index;
        }

        // 新規割り当て可能な容量があるか確認
        assert(nextDescriptorIndex_ < descriptorCount_);

        if (nextDescriptorIndex_ >= descriptorCount_) {
            return UINT32_MAX;
        }

        const uint32_t index = nextDescriptorIndex_++;

        assert(!isDescriptorAllocated_[index]);

        isDescriptorAllocated_[index] = true;

        return index;
    }

    void DescriptorHeap::FreeDescriptor(uint32_t index) {

        // 予約領域や範囲外のインデックスは解放しない
        assert(index > 0);
        assert(index < nextDescriptorIndex_);
        assert(index < descriptorCount_);

        if (index == 0 ||
            index >= nextDescriptorIndex_ ||
            index >= descriptorCount_) {
            return;
        }

        // 二重解放を防ぐ
        assert(isDescriptorAllocated_[index]);

        if (!isDescriptorAllocated_[index]) {
            return;
        }

        // 未割り当て状態に戻し、再利用リストへ追加
        isDescriptorAllocated_[index] = false;

        freeDescriptorIndices_.push_back(index);
    }
}