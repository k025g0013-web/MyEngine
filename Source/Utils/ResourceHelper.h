#pragma once
#include <d3d12.h>
#include <cassert>

/// <summary>
/// アップロードヒープ上にバッファリソースを生成する
/// </summary>
/// <param name="device">
/// Direct3Dデバイス
/// </param>
/// <param name="sizeInBytes">
/// 作成するバッファサイズ(Byte)
/// </param>
/// <returns>
/// 作成したバッファリソース
/// </returns>
/// <remarks>
/// CPUからGPUへデータを転送するためのアップロードヒープを利用する。
/// 頂点バッファや定数バッファなど、CPUから頻繁に更新する
/// リソースの生成に使用する。
/// </remarks>
inline ID3D12Resource *CreateBufferResource(ID3D12Device *device, size_t sizeInBytes) {

    // CPUからGPUへ書き込み可能なアップロードヒープを使用する
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

    // バッファリソースの設定を行う
    D3D12_RESOURCE_DESC resourceDesc{};

    // バッファリソースとして生成する
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

    // 必要なバイト数を設定
    resourceDesc.Width = sizeInBytes;

    // バッファでは高さ・配列数・MipMap数は固定値
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;

    // マルチサンプリングは使用しない
    resourceDesc.SampleDesc.Count = 1;

    // バッファはROW_MAJORレイアウト固定
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ID3D12Resource *resource = nullptr;

    HRESULT hr =
        device->CreateCommittedResource(
            &uploadHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&resource));

    // リソース生成に失敗した場合は停止
    assert(SUCCEEDED(hr));
    (void)hr;

    return resource;
}