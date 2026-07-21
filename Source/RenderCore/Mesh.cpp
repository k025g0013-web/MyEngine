#include "Mesh.h"

#include "Renderer/Renderer.h"

namespace Kizuna {
    void Mesh::Create(
        ID3D12Device *device,
        const std::vector<VertexData> &vertices
    ) {
        // 描画に使用する頂点数を保持する
        vertexCount_ = static_cast<uint32_t>(vertices.size());

        // 頂点データからGPU用頂点バッファを生成する
        Renderer render;
        render.CreateVertexBuffer(device, vertexBuffer_, vertices);

        // インデックスバッファを使用しないメッシュとして扱う
        hasIndexBuffer_ = false;
    }

    void Mesh::Create(
        ID3D12Device *device,
        const std::vector<VertexData> &vertices,
        const std::vector<uint32_t> &indices
    ) {
        // 頂点数・インデックス数を保持する
        vertexCount_ = static_cast<uint32_t>(vertices.size());
        indexCount_ = static_cast<uint32_t>(indices.size());

        Renderer render;

        // GPU用頂点バッファを生成する
        render.CreateVertexBuffer(device, vertexBuffer_, vertices);

        // GPU用インデックスバッファを生成する
        render.CreateIndexBuffer(device, indexBuffer_, indices);

        // インデックス描画を行うメッシュとして扱う
        hasIndexBuffer_ = true;
    }

    void Mesh::Bind(ID3D12GraphicsCommandList *commandList) {

        // 頂点バッファを入力アセンブラへ設定する
        commandList->IASetVertexBuffers(0, 1, vertexBuffer_.GetVertexBufferView());

        if (hasIndexBuffer_) {

            // インデックスバッファを設定する
            commandList->IASetIndexBuffer(indexBuffer_.GetIndexBufferView());

            // インデックスバッファを利用して描画する
            commandList->DrawIndexedInstanced(indexCount_, 1, 0, 0, 0);

        } else {

            // 頂点バッファのみを利用して描画する
            commandList->DrawInstanced(vertexCount_, 1, 0, 0);
        }
    }
}