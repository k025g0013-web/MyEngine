#pragma once

#pragma comment(lib, "d3d12.lib")

#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

#include "Math/Vector.h"

namespace Kizuna {
    /// <summary>
    /// 頂点データ構造体
    /// </summary>
    /// <remarks>
    /// モデルの頂点座標、UV座標、法線ベクトルを保持する。
    /// </remarks>
    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };

    /// <summary>
    /// GPUバッファを管理するクラス
    /// </summary>
    /// <remarks>
    /// VertexBuffer、IndexBuffer、ConstantBufferの生成・管理を行い、
    /// GPUへ送るデータの書き込みやBufferViewの取得を提供する。
    /// </remarks>
    class MeshBuffer {
    public:
        /// <summary>
        /// バッファを解放する
        /// </summary>
        ~MeshBuffer();

        /// <summary>
        /// VertexBufferを初期化する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="sizeInBytes">バッファサイズ</param>
        /// <param name="strideInBytes">頂点1つあたりのサイズ</param>
        void InitializeAsVertex(ID3D12Device *device, size_t sizeInBytes, uint32_t strideInBytes);

        /// <summary>
        /// IndexBufferを初期化する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="sizeInBytes">バッファサイズ</param>
        void InitializeAsIndex(ID3D12Device *device, size_t sizeInBytes);

        /// <summary>
        /// ConstantBufferを初期化する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="sizeInBytes">バッファサイズ</param>
        void InitializeAsConstant(ID3D12Device *device, size_t sizeInBytes);

        /// <summary>
        /// バッファをCPUから書き込み可能な状態にする
        /// </summary>
        /// <returns>マッピングされたメモリアドレス</returns>
        void *Map();

        /// <summary>
        /// バッファのマッピングを解除する
        /// </summary>
        void Unmap();

        /// <summary>
        /// GPUリソースを取得する
        /// </summary>
        /// <returns>GPUリソース</returns>
        ID3D12Resource *GetResource() const { return resource_.Get(); }

        /// <summary>
        /// VertexBufferViewを取得する
        /// </summary>
        /// <returns>VertexBufferView</returns>
        const D3D12_VERTEX_BUFFER_VIEW *GetVertexBufferView() const { return &vertexBufferView_; }

        /// <summary>
        /// IndexBufferViewを取得する
        /// </summary>
        /// <returns>IndexBufferView</returns>
        const D3D12_INDEX_BUFFER_VIEW *GetIndexBufferView() const { return &indexBufferView_; }

        /// <summary>
        /// ConstantBufferのGPU仮想アドレスを取得する
        /// </summary>
        /// <returns>GPU仮想アドレス</returns>
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

    private:
        /// GPUバッファリソース
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

        /// CPUからアクセスするためのマッピングアドレス
        void *mappedData_ = nullptr;

        /// VertexBufferView
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

        /// IndexBufferView
        D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
    };
}