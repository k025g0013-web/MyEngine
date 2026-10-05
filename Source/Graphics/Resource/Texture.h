#pragma once


#include <d3d12.h>
#include <wrl.h>
#include <DirectXTex/DirectXTex.h>

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <string>

namespace Kizuna {
	class DescriptorHeap;

	/// <summary>
	/// GPU上のテクスチャを参照するための情報
	/// </summary>
	/// <remarks>
	/// CPU・GPUディスクリプタハンドル、テクスチャ情報、
	/// GPUリソースへのポインタを保持する。
	/// </remarks>
	struct TextureData {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
		DirectX::TexMetadata metadata{};
		ID3D12Resource *resource = nullptr;
	};

	/// <summary>
	/// テクスチャリソースを管理するクラス
	/// </summary>
	/// <remarks>
	/// WIC画像の読み込み、MipMap生成、GPUリソース作成、
	/// SRV生成およびテクスチャのキャッシュ管理を行う。
	/// 同じテクスチャは一度だけ読み込み、再利用する。
	/// </remarks>
	class Texture {
	public:

		/// <summary>
		/// テクスチャシステムを初期化する
		/// </summary>
		/// <param name="device">DirectXデバイス</param>
		/// <param name="srvHeap">SRV用DescriptorHeap</param>
		void Initialize(ID3D12Device *device, DescriptorHeap *srvHeap);

		/// <summary>
		/// テクスチャシステムを終了する
		/// </summary>
		void Finalize();

		/// <summary>
		/// テクスチャを読み込む
		/// </summary>
		/// <param name="commandList">GPU転送に使用するコマンドリスト</param>
		/// <param name="textureName">読み込むテクスチャファイル名</param>
		/// <returns>読み込んだテクスチャ情報</returns>
		TextureData LoadTexture(ID3D12GraphicsCommandList *commandList, const std::string &textureName);

		/// <summary>
		/// テクスチャのGPUハンドルを取得する
		/// </summary>
		/// <param name="textureName">テクスチャ名</param>
		/// <returns>GPUディスクリプタハンドル</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const std::string &textureName) const {
			auto it = textures_.find(textureName);
			if (it != textures_.end()) {
				return it->second->gpuHandle;
			}
			assert(false && "指定されたテクスチャは読み込まれていません");
			return D3D12_GPU_DESCRIPTOR_HANDLE{};
		}

		/// <summary>
		/// テクスチャリソースを取得する
		/// </summary>
		/// <param name="textureName">テクスチャ名</param>
		/// <returns>GPUリソース</returns>
		ID3D12Resource *GetResource(const std::string &textureName) const {
			auto it = textures_.find(textureName);
			if (it != textures_.end()) {
				return it->second->resource.Get();
			}
			assert(false && "指定されたテクスチャは読み込まれていません");
			return nullptr;
		}

	private:

		/// <summary>
		/// 管理用テクスチャ情報
		/// </summary>
		/// <remarks>
		/// GPUリソース、アップロードバッファ、
		/// SRVハンドルなど永続的に保持する情報を格納する。
		/// </remarks>
		struct ResourceRecord {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
			Microsoft::WRL::ComPtr<ID3D12Resource> intermediate = nullptr;
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
			DirectX::TexMetadata metadata{};
		};

		/// <summary>
		/// テクスチャ読み込み時の作業情報
		/// </summary>
		/// <remarks>
		/// 読み込み中のみ使用する一時データを保持する。
		/// </remarks>
		struct LoadContext {
			DirectX::ScratchImage mipImages{};
			DirectX::TexMetadata metadata{};
			Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
			Microsoft::WRL::ComPtr<ID3D12Resource> intermediate = nullptr;
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
		};

		/// <summary>
		/// WIC画像を読み込みMipMapを生成する
		/// </summary>
		/// <param name="textureName">テクスチャ名</param>
		/// <param name="context">読み込み情報</param>
		/// <returns>成功した場合はtrue</returns>
		bool LoadWICAndGenerateMips(const std::string &textureName, LoadContext &context);

		/// <summary>
		/// GPU上にテクスチャリソースを生成する
		/// </summary>
		/// <param name="context">読み込み情報</param>
		/// <returns>成功した場合はtrue</returns>
		bool CreateTextureResource(LoadContext &context);

		/// <summary>
		/// テクスチャデータをGPUへ転送する
		/// </summary>
		/// <param name="commandList">コマンドリスト</param>
		/// <param name="context">読み込み情報</param>
		void UploadTextureData(ID3D12GraphicsCommandList *commandList, LoadContext &context);

		/// <summary>
		/// Shader Resource View(SRV)を生成する
		/// </summary>
		/// <param name="context">読み込み情報</param>
		void CreateSRV(LoadContext &context);

	private:

		/// DirectXデバイス
		ID3D12Device *device_{};

		/// SRV用DescriptorHeap
		DescriptorHeap *srvHeap_{};

		/// 読み込み済みテクスチャ一覧
		std::map<std::string, std::unique_ptr<ResourceRecord>> textures_;

		/// 次に割り当てるSRVディスクリプタ番号
		static uint32_t nextDescriptorIndex_;
	};
}