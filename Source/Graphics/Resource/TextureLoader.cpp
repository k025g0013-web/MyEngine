#include "TextureLoader.h"

#include <cassert>

#include <DirectXTex/d3dx12.h>

#include "Graphics/Core/DescriptorHeap.h"
#include "Utils/ConvertString.h"
#include "Utils/ResourceHelper.h"

namespace Kizuna {

    // 次に割り当てるSRVディスクリプタ番号
    uint32_t TextureLoader::nextDescriptorIndex_ = 1;

    void TextureLoader::Initialize(
        ID3D12Device *device, DescriptorHeap *srvHeap) {
        
        // デバイスとSRVヒープを保持する
        assert(device);
        assert(srvHeap);

        device_ = device;
        srvHeap_ = srvHeap;
    }

    TextureData TextureLoader::LoadTexture(
        ID3D12GraphicsCommandList *commandList,
        const std::string &textureName, Texture &texture) {

        // TextureLoaderが初期化されていることを確認する
        assert(device_ && srvHeap_ &&
            "TextureLoaderが初期化されていません");

        // テクスチャ読み込み用の作業データ
        LoadContext context{};

        // テクスチャファイルを読み込み、
        // MipMapを生成する
        bool success = LoadWICAndGenerateMips(textureName, context);

        assert(success && "テクスチャファイルの読み込みに失敗しました");

        // GPU上にテクスチャリソースを作成する
        success = CreateTextureResource(context);

        assert(success && "テクスチャリソースの作成に失敗しました");

        // テクスチャデータをGPUへ転送する
        UploadTextureData(commandList, context);

        // ShaderResourceViewを生成する
        CreateSRV(context);

        // 作成したGPUリソースをTextureへ渡す
        texture.SetResource(std::move(context.resource));

        // GPU転送用の中間バッファをTextureへ渡す
        texture.SetIntermediate(std::move(context.intermediate));

        // テクスチャ情報を作成する
        TextureData data{};
        data.cpuHandle = context.cpuHandle;
        data.gpuHandle = context.gpuHandle;
        data.metadata = context.metadata;

        // Texture側で所有している
        // GPUリソースを設定する
        data.resource = texture.GetResource();

        // テクスチャ情報をTextureへ設定する
        texture.SetData(data);

        return data;
    }

    bool TextureLoader::LoadWICAndGenerateMips(
        const std::string &textureName,
        LoadContext &context) {

        // ファイルパスをワイド文字列へ変換する
        std::string filePath = textureName;

        std::wstring filePathW = ConvertString(filePath);

        // WICを利用して画像を読み込む
        DirectX::ScratchImage image{};

        HRESULT hr = DirectX::LoadFromWICFile(
                filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

        if (FAILED(hr)) {
            return false;
        }

        // MipMapを生成する
        hr = DirectX::GenerateMipMaps(
            image.GetImages(), image.GetImageCount(), image.GetMetadata(),
            DirectX::TEX_FILTER_SRGB, 0, context.mipImages);

        if (FAILED(hr)) {

            // MipMap生成に失敗した場合は
            // 元画像をそのまま使用する
            context.mipImages = std::move(image);
        }

        // テクスチャ情報を保存する
        context.metadata = context.mipImages.GetMetadata();

        return true;
    }

    bool TextureLoader::CreateTextureResource(
        LoadContext &context) {

        // テクスチャリソース情報を設定する
        D3D12_RESOURCE_DESC resourceDesc{};

        resourceDesc.Width = UINT(context.metadata.width);
        resourceDesc.Height = UINT(context.metadata.height);

        resourceDesc.MipLevels = UINT16(context.metadata.mipLevels);
        resourceDesc.DepthOrArraySize = UINT16(context.metadata.arraySize);

        resourceDesc.Format = context.metadata.format;

        resourceDesc.SampleDesc.Count = 1;

        resourceDesc.Dimension =
            D3D12_RESOURCE_DIMENSION(context.metadata.dimension);

        // デフォルトヒープを設定する
        D3D12_HEAP_PROPERTIES heapProperties{};

        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        // GPU上にテクスチャリソースを生成する
        HRESULT hr = device_->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE,
            &resourceDesc,D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr, IID_PPV_ARGS(&context.resource));

        return SUCCEEDED(hr);
    }

    void TextureLoader::UploadTextureData(
        ID3D12GraphicsCommandList *commandList, LoadContext &context) {

        // サブリソース情報を作成する
        std::vector<D3D12_SUBRESOURCE_DATA> subresources;

        DirectX::PrepareUpload(device_, 
            context.mipImages.GetImages(), context.mipImages.GetImageCount(),
            context.metadata, subresources);

        // 中間バッファを生成する
        uint64_t intermediateSize =
            GetRequiredIntermediateSize(context.resource.Get(), 0, UINT(subresources.size()));

        context.intermediate =
            CreateBufferResource(device_, intermediateSize);

        // テクスチャデータをGPUへコピーする
        UpdateSubresources(commandList,
            context.resource.Get(), context.intermediate.Get(),
            0, 0, UINT(subresources.size()), subresources.data());

        // コピー完了後、
        // シェーダーから参照可能な状態へ遷移する
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        
        barrier.Transition.pResource = context.resource.Get();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

        commandList->ResourceBarrier(1, &barrier);
    }

    void TextureLoader::CreateSRV(LoadContext &context) {

        // 使用するディスクリプタ番号を取得する
        uint32_t descriptorIndex = nextDescriptorIndex_;

        nextDescriptorIndex_++;

        // CPU/GPUディスクリプタハンドルを取得する
        context.cpuHandle = srvHeap_->GetCPUDescriptorHandle(descriptorIndex);
        context.gpuHandle = srvHeap_->GetGPUDescriptorHandle(descriptorIndex);

        // SRV情報を設定する
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = context.metadata.format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = UINT(context.metadata.mipLevels);

        // ShaderResourceViewを生成する
        device_->CreateShaderResourceView(context.resource.Get(), &srvDesc, context.cpuHandle);
    }
}