#include "Texture.h"
#include <cassert>
#include <DirectXTex/d3dx12.h>

#include "Graphics/Core/DescriptorHeap.h" // DescriptorHeapの定義ヘッダー
#include "Utils/ConvertString.h"           // ConvertStringの定義ヘッダー
#include "Utils/ResourceHelper.h"          // CreateBufferResourceの定義ヘッダー

uint32_t Texture::nextDescriptorIndex_ = 1;

void Texture::Initialize(ID3D12Device *device, DescriptorHeap *srvHeap) {
	assert(device);
	assert(srvHeap);
	device_ = device;
	srvHeap_ = srvHeap;
}

void Texture::Finalize() {
	// すべてのComPtrをクリア
	textures_.clear();
	device_ = nullptr;
	srvHeap_ = nullptr;
}

TextureData Texture::LoadTexture(ID3D12GraphicsCommandList *commandList, const std::string &textureName) {
	assert(device_ && srvHeap_ && "Textureシステムが初期化されていません");

	// すでに読み込み済みかマップ内を検索
	auto it = textures_.find(textureName);
	if (it != textures_.end()) {
		TextureData data;
		data.cpuHandle = it->second->cpuHandle;
		data.gpuHandle = it->second->gpuHandle;
		data.metadata = it->second->metadata;
		data.resource = it->second->resource.Get();
		return data;
	}

	// 内部処理用の文脈を生成
	LoadContext context{};

	// ファイル読み込み/ミップマップ生成
	bool success = LoadWICAndGenerateMips(textureName, context);
	assert(success && "テクスチャファイルの読み込みに失敗しました");

	// VRAM上にリソース確保
	success = CreateTextureResource(context);
	assert(success && "テクスチャリソースの作成に失敗しました");

	// GPUへのデータ転送
	UploadTextureData(commandList, context);

	// SRV作成
	CreateSRV(context);

	// マネージャへの登録処理
	auto record = std::make_unique<ResourceRecord>();
	record->resource = std::move(context.resource);
	record->intermediate = std::move(context.intermediate);
	record->cpuHandle = context.cpuHandle;
	record->gpuHandle = context.gpuHandle;
	record->metadata = context.metadata;

	TextureData data;
	data.cpuHandle = record->cpuHandle;
	data.gpuHandle = record->gpuHandle;
	data.metadata = record->metadata;
	data.resource = record->resource.Get();

	textures_[textureName] = std::move(record);

	return data;
}

bool Texture::LoadWICAndGenerateMips(const std::string &textureName, LoadContext &context) {
	std::string filePath = textureName;
	std::wstring filePathW = ConvertString(filePath);

	DirectX::ScratchImage image{};
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	if (FAILED(hr)) return false;

	hr = DirectX::GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0, context.mipImages
	);
	if (FAILED(hr)) return false;

	context.metadata = context.mipImages.GetMetadata();
	return true;
}

bool Texture::CreateTextureResource(LoadContext &context) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(context.metadata.width);
	resourceDesc.Height = UINT(context.metadata.height);
	resourceDesc.MipLevels = UINT16(context.metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(context.metadata.arraySize);
	resourceDesc.Format = context.metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(context.metadata.dimension);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&context.resource)
	);
	return SUCCEEDED(hr);
}

void Texture::UploadTextureData(ID3D12GraphicsCommandList *commandList, LoadContext &context) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(
		device_, context.mipImages.GetImages(), context.mipImages.GetImageCount(), context.metadata, subresources);

	uint64_t intermediateSize = GetRequiredIntermediateSize(context.resource.Get(), 0, UINT(subresources.size()));
	context.intermediate = CreateBufferResource(device_, intermediateSize);

	UpdateSubresources(
		commandList, context.resource.Get(), context.intermediate.Get(),
		0, 0, UINT(subresources.size()), subresources.data()
	);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = context.resource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
}

void Texture::CreateSRV(LoadContext &context) {
	uint32_t descriptorIndex = nextDescriptorIndex_;
	nextDescriptorIndex_++;

	context.cpuHandle = srvHeap_->GetCPUDescriptorHandle(descriptorIndex);
	context.gpuHandle = srvHeap_->GetGPUDescriptorHandle(descriptorIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = context.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(context.metadata.mipLevels);

	device_->CreateShaderResourceView(context.resource.Get(), &srvDesc, context.cpuHandle);
}