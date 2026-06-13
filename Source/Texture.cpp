#include "Texture.h"

#include <cassert>

#include <DirectXTex/d3dx12.h>

#include "DescriptorHeap.h"

#include "Utils/ConvertString.h"
#include "Utils/ResourceHelper.h"

uint32_t Texture::nextDescriptorIndex_ = 1;

void Texture::Initialize(
	ID3D12Device *device, ID3D12GraphicsCommandList *commandList,
	DescriptorHeap *srvHeap, const std::string &filePath
) {
	filePath_ = filePath;

	uint32_t descriptorIndex = nextDescriptorIndex_;
	nextDescriptorIndex_++;

	// Texture読み込み
	LoadTexture();

	// Resource生成
	CreateTextureResource(device);

	// Upload
	UploadTextureData(device, commandList);

	// SRVの作成
	CreateSRV(device, srvHeap, descriptorIndex);
}

// Texture読み込み
void Texture::LoadTexture() {
	// テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath_);
	HRESULT hr = DirectX::LoadFromWICFile(	// 画像のロード
		filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップ生成
	hr = DirectX::GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), image.GetMetadata(), 
		DirectX::TEX_FILTER_SRGB, 0, mipImages_
	);
	assert(SUCCEEDED(hr));
	(void)hr;

	// ミニマップ付きのデータを入力する
	metadata_ = mipImages_.GetMetadata();
}

// TextureResource作成
void Texture::CreateTextureResource(ID3D12Device *device) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata_.width);		// Textureの幅
	resourceDesc.Height = UINT(metadata_.height);	// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata_.mipLevels);			// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata_.arraySize);	// 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata_.format;	// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;		// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata_.dimension);	// Textureの次元数。普段使っているのは2次元

	// 利用するHeapの設定。VRAM上に作成する
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceを生成する
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, // データ転送される設定
		nullptr, // Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&textureResource_) // 作成するResourceポインタへのポインタ
	);
	(void)hr;

	assert(SUCCEEDED(hr));
}

// UploadTextureDataを書き換える
void Texture::UploadTextureData(
	ID3D12Device *device, ID3D12GraphicsCommandList *commandList
) {
	// 各ミップレベルの転送情報を構築
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;			// IntermediateResource(中間リソース)
	DirectX::PrepareUpload(
		device, mipImages_.GetImages(), mipImages_.GetImageCount(), mipImages_.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(	// Upload Heapサイズ算出
		textureResource_.Get(), 0, UINT(subresources.size()));
	intermediateResource_ =										// 中間バッファ（Upload用リソース）
		CreateBufferResource(device, intermediateSize);

	// データ転送をコマンドに積む
	UpdateSubresources(
		commandList, textureResource_.Get(), intermediateResource_.Get(),
		0, 0, UINT(subresources.size()), subresources.data()
	);

	// Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = textureResource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
}

// SRV生成
void Texture::CreateSRV(
	ID3D12Device *device, DescriptorHeap *srvHeap, uint32_t descriptorIndex
) {
	// CPU/GPU両方のハンドル計算
	cpuHandle_ = srvHeap->GetCPUDescriptorHandle(descriptorIndex);
	gpuHandle_ = srvHeap->GetGPUDescriptorHandle(descriptorIndex);

	// SRV設定（シェーダ用ビュー定義）
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata_.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata_.mipLevels);

	// GPUにSRV登録
	device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, cpuHandle_);
}