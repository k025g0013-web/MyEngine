#pragma once

#include <vector>

#include "MeshBuffer.h"

namespace Kizuna {
    /// <summary>
    /// メッシュデータを管理するクラス
    /// </summary>
    /// <remarks>
    /// 頂点バッファおよびインデックスバッファを保持し、
    /// GPUへバインドして描画を行う。
    /// インデックスバッファの有無に応じて描画方法を切り替える。
    /// </remarks>
    class Mesh {
    public:

        /// <summary>
        /// 頂点データのみを使用したメッシュを生成する
        /// </summary>
        /// <param name="device">Direct3Dデバイス</param>
        /// <param name="vertices">頂点データ</param>
        void Create(
            ID3D12Device *device,
            const std::vector<VertexData> &vertices
        );

        /// <summary>
        /// 頂点データとインデックスデータを使用したメッシュを生成する
        /// </summary>
        /// <param name="device">Direct3Dデバイス</param>
        /// <param name="vertices">頂点データ</param>
        /// <param name="indices">インデックスデータ</param>
        void Create(
            ID3D12Device *device,
            const std::vector<VertexData> &vertices,
            const std::vector<uint32_t> &indices
        );

        /// <summary>
        /// メッシュをGPUへバインドし描画する
        /// </summary>
        /// <param name="commandList">描画コマンドリスト</param>
        void Bind(ID3D12GraphicsCommandList *commandList);

        /// <summary>
        /// 頂点数を取得する
        /// </summary>
        uint32_t GetVertexCount() const { return vertexCount_; }

        /// <summary>
        /// インデックス数を取得する
        /// </summary>
        uint32_t GetIndexCount() const { return indexCount_; }

        /// <summary>
        /// インデックスバッファを保持しているか判定する
        /// </summary>
        bool HasIndexBuffer() const { return hasIndexBuffer_; }

    private:

        /// 頂点バッファ
        MeshBuffer vertexBuffer_;

        /// インデックスバッファ
        MeshBuffer indexBuffer_;

        /// 頂点数
        uint32_t vertexCount_ = 0;

        /// インデックス数
        uint32_t indexCount_ = 0;

        /// インデックスバッファを保持しているか
        bool hasIndexBuffer_ = false;
    };
}