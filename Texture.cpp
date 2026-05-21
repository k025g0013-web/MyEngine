#include "Texture.h"

#include <cassert>

#include <DirectXTex/d3dx12.h>

#include "ConvertString.h"
#include "ResourceUtils.h"

void Texture::Initialize(
	ID3D12Device *device, ID3D12GraphicsCommandList *commandList, ID3D12DescriptorHeap *srvHeap,
	uint32_t descriptorSize, uint32_t descriptorIndex, const std::string &filePath
) {
	filePath_ = filePath;

	// Texture読み込み
	mipImages_ = LoadTexture(filePath_);	
	metadata_ = mipImages_.GetMetadata();

	// Resource生成
	textureResource_ = CreateTextureResource(device, metadata_);	
	
	// Upload
	intermediateResource_ = UploadTextureData(textureResource_.Get(), mipImages_, device, commandList);	

	// SRVの作成
	CreateSRV(device, srvHeap, descriptorSize, descriptorIndex);
}

// Texture読み込み
DirectX::ScratchImage Texture::LoadTexture(const std::string &filePath) {
	DirectX::ScratchImage image{};

	std::wstring filePathW = ConvertString(filePath);

	// 画像のロード
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップ生成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0, mipImages
	);
	assert(SUCCEEDED(hr));

	return mipImages;
}

// TextureResource作成
Microsoft::WRL::ComPtr<ID3D12Resource> Texture::CreateTextureResource(
	ID3D12Device *device, const DirectX::TexMetadata &metadata
) {
	// テクスチャの仕様をGPU用に構築
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// VRAM上に確保
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource;

	// コピー先として初期状態をCOPY_DESTにする
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	return resource;
}

// CPU→GPUテクスチャ転送
Microsoft::WRL::ComPtr<ID3D12Resource> Texture::UploadTextureData(
	ID3D12Resource *texture, const DirectX::ScratchImage &mipImages,
	ID3D12Device *device, ID3D12GraphicsCommandList *commandList
) {
	// 各ミップレベルの転送情報を構築
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(
		device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources
	);

	// Upload Heapサイズ算出
	uint64_t intermediateSize = GetRequiredIntermediateSize(
		texture, 0, UINT(subresources.size()));

	// 中間バッファ（Upload用リソース）
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource =
		CreateBufferResource(device, intermediateSize);

	// コマンドリストにコピー命令を積む
	UpdateSubresources(
		commandList, texture, intermediateResource.Get(),
		0, 0, UINT(subresources.size()), subresources.data()
	);

	// コピー完了後の状態へ遷移（COPY_DEST → SHADER_READ）
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

void Texture::CreateSRV(
	ID3D12Device *device, ID3D12DescriptorHeap *srvHeap, uint32_t descriptorSize, uint32_t descriptorIndex
) {
	// CPU/GPU両方のハンドル計算
	cpuHandle_ = srvHeap->GetCPUDescriptorHandleForHeapStart();
	cpuHandle_.ptr += descriptorSize * descriptorIndex;

	gpuHandle_ = srvHeap->GetGPUDescriptorHandleForHeapStart();
	gpuHandle_.ptr += descriptorSize * descriptorIndex;

	// SRV設定（シェーダ用ビュー定義）
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata_.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata_.mipLevels);

	// GPUにSRV登録
	device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, cpuHandle_);
}