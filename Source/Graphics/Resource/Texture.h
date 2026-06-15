#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <DirectXTex/DirectXTex.h>

#include <map>
#include <string>
#include <fstream>

class DescriptorHeap;

// テクスチャへの参照用ハンドル
struct TextureData {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	DirectX::TexMetadata metadata;
	ID3D12Resource *resource = nullptr;
};

class Texture {
public:
	void Initialize(ID3D12Device *device, DescriptorHeap *srvHeap);
	void Finalize();

	// テクスチャの読み込み
	TextureData LoadTexture(ID3D12GraphicsCommandList *commandList, const std::string &textureName);

	// getter
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const std::string &textureName) const {
		auto it = textures_.find(textureName);
		if (it != textures_.end()) {
			return it->second->gpuHandle;
		}
		assert(false && "指定されたテクスチャは読み込まれていません");
		return D3D12_GPU_DESCRIPTOR_HANDLE{};
	}

	ID3D12Resource *GetResource(const std::string &textureName) const { 
		auto it = textures_.find(textureName);
		if (it != textures_.end()) {
			return it->second->resource.Get();
		}
		assert(false && "指定されたテクスチャは読み込まれていません");
		return nullptr;
	}

private:
	struct ResourceRecord {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediate;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		DirectX::TexMetadata metadata;
	};

	struct LoadContext {
		DirectX::ScratchImage mipImages;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediate;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};

	bool LoadWICAndGenerateMips(const std::string &textureName, LoadContext &context);
	bool CreateTextureResource(LoadContext &context);
	void UploadTextureData(ID3D12GraphicsCommandList *commandList, LoadContext &context);
	void CreateSRV(LoadContext &context);

private:
	ID3D12Device *device_{};
	DescriptorHeap *srvHeap_{};

	// テクスチャ管理
	std::map<std::string, std::unique_ptr<ResourceRecord>> textures_;

	static uint32_t nextDescriptorIndex_;
};