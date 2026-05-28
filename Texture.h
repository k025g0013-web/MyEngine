#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXTex/DirectXTex.h>

#include <string>


class DescriptorHeap;
class Texture {
public:
	void Initialize(
		ID3D12Device *device, ID3D12GraphicsCommandList *commandList, 
		DescriptorHeap *srvHeap, const std::string &filePath
	);

	// getter
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const {return gpuHandle_;}
	ID3D12Resource *GetResource() const {return textureResource_.Get();}

private:
	// Texture生成
	void LoadTexture();

	// TextureResource作成
	void CreateTextureResource(ID3D12Device *device);

	// UploadTextureDataを書き換える
	void UploadTextureData(
		ID3D12Device *device, ID3D12GraphicsCommandList *commandList
	);

	// SRV生成
	void CreateSRV(
		ID3D12Device *device, DescriptorHeap *srvHeap, uint32_t descriptorIndex
	);

private:
	std::string filePath_;

	DirectX::ScratchImage mipImages_;
	DirectX::TexMetadata metadata_{};

	static uint32_t nextDescriptorIndex_;

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};
};