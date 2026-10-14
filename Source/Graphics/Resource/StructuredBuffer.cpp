
#include "StructuredBuffer.h"

#include <cassert>
#include <cstring>
#include <limits>

#include "Graphics/Core/DescriptorHeap.h"

namespace Kizuna {

    void StructuredBuffer::Initialize(
        ID3D12Device *device,
        DescriptorHeap *srvHeap,
        uint32_t descriptorIndex,
        uint32_t elementSize,
        uint32_t elementCount
    ) {
        // 引数が正しく渡されているか確認する
        assert(device);
        assert(srvHeap);
        assert(elementSize > 0);
        assert(elementCount > 0);

        // 既に初期化済みなら再初期化しない
        assert(!resource_);

        // バッファ全体のサイズを計算する
        // 例：128バイトの構造体を100個格納する場合は12800バイト
        const uint64_t bufferSize =
            static_cast<uint64_t>(elementSize) * elementCount;

        // バッファサイズが0や上限超過にならないことを確認する
        assert(bufferSize > 0);
        assert(bufferSize <= 
            static_cast<uint64_t>((std::numeric_limits<SIZE_T>::max)())
        );

        // 要素サイズと要素数を保存する
        elementSize_ = elementSize;
        elementCount_ = elementCount;

        //========================================
        // 1. GPUリソースの設定
        //========================================

        D3D12_RESOURCE_DESC resourceDesc{};

        // 構造化バッファはBufferとして作成する
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

        // BufferではHeightは1
        resourceDesc.Height = 1;

        // BufferではDepthOrArraySizeは1
        resourceDesc.DepthOrArraySize = 1;

        // BufferではMipLevelsは1
        resourceDesc.MipLevels = 1;

        // 構造化バッファのFormatはSRV側でUNKNOWNにする
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;

        // BufferのSampleDescは通常この設定
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;

        // Bufferのサイズをバイト単位で指定する
        resourceDesc.Width = bufferSize;

        // BufferはROW_MAJORを指定する
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // UAVとして使用しないため、追加フラグは不要
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


        //========================================
        // 2. CPUから書き込めるHeapを設定
        //========================================

        D3D12_HEAP_PROPERTIES heapProperties{};

        // Upload Heapを使用する
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

        // その他のプロパティは既定値
        heapProperties.CPUPageProperty =
            D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

        heapProperties.MemoryPoolPreference =
            D3D12_MEMORY_POOL_UNKNOWN;

        heapProperties.CreationNodeMask = 1;
        heapProperties.VisibleNodeMask = 1;


        //========================================
        // 3. GPUリソースを作成
        //========================================

        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&resource_)
        );

        assert(SUCCEEDED(hr));

        if (FAILED(hr)) {
            return;
        }


        //========================================
        // 4. CPUからアクセスできるようにMap
        //========================================

        // 読み取り範囲は指定しない
        // CPUから書き込む用途なので読み取り範囲は空にする
        D3D12_RANGE readRange{};
        readRange.Begin = 0;
        readRange.End = 0;

        hr = resource_->Map(
            0,
            &readRange,
            &mappedData_
        );

        assert(SUCCEEDED(hr));

        if (FAILED(hr)) {
            resource_.Reset();
            return;
        }


        //========================================
        // 5. SRVを作成
        //========================================

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

        // シェーダーの標準的なコンポーネントマッピング
        srvDesc.Shader4ComponentMapping =
            D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        // 構造化バッファのSRVを指定
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

        // 構造化バッファではUNKNOWNを指定する
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;

        // 先頭要素から参照する
        srvDesc.Buffer.FirstElement = 0;

        // 参照する要素数
        srvDesc.Buffer.NumElements = elementCount_;

        // 1要素のバイト数
        srvDesc.Buffer.StructureByteStride = elementSize_;

        // 通常のSRV
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;


        // 指定されたDescriptor番号のCPUハンドルを取得
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle =
            srvHeap->GetCPUDescriptorHandle(descriptorIndex);

        // SRVをDescriptorHeapに登録
        device->CreateShaderResourceView(
            resource_.Get(),
            &srvDesc,
            cpuHandle
        );

        // 描画時に使うGPUハンドルを保存
        gpuHandle_ =
            srvHeap->GetGPUDescriptorHandle(descriptorIndex);
    }


    void StructuredBuffer::Update(
        const void *data,
        size_t dataSize
    ) {
        // 初期化されていることを確認
        assert(resource_);
        assert(mappedData_);
        assert(data);

        // バッファの容量を超えるデータは書き込めない
        const uint64_t bufferSize =
            static_cast<uint64_t>(elementSize_) * elementCount_;

        assert(dataSize <= bufferSize);

        if (!mappedData_ || !data || dataSize > bufferSize) {
            return;
        }

        // CPU側のデータをバッファへコピーする
        std::memcpy(
            mappedData_,
            data,
            dataSize
        );
    }
}
