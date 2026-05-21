#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXTex/DirectXTex.h>

#include <string>

class Texture {
public:
	void Initialize(
		ID3D12Device *device, ID3D12GraphicsCommandList *commandList, ID3D12DescriptorHeap *srvHeap, 
		uint32_t descriptorSize, uint32_t descriptorIndex, const std::string &filePath
	);

	// getter
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const {return gpuHandle_;}
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const {return cpuHandle_;}
	ID3D12Resource *GetResource() const {return textureResource_.Get();}

private:
	// Texture読み込み
	DirectX::ScratchImage LoadTexture(const std::string &filePath);

	// TextureResource作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
		ID3D12Device *device, const DirectX::TexMetadata &metadata
	);

	// CPU→GPUテクスチャ転送
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(
		ID3D12Resource *texture, const DirectX::ScratchImage &mipImages,
		ID3D12Device *device, ID3D12GraphicsCommandList *commandList
	);

	// SRV生成
	void CreateSRV(
		ID3D12Device *device, ID3D12DescriptorHeap *srvHeap,
		uint32_t descriptorSize, uint32_t descriptorIndex
	);

private:
	std::string filePath_;

	DirectX::ScratchImage mipImages_;

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};

	DirectX::TexMetadata metadata_{};
};