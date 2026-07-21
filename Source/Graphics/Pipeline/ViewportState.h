#pragma once

#include <d3d12.h>

namespace Kizuna {
    /// <summary>
    /// DirectX12のViewportとScissorRectを管理するクラス
    /// </summary>
    /// <remarks>
    /// 描画領域のサイズや範囲を設定し、
    /// コマンドリストへViewportおよびScissorRectの設定を行う。
    /// </remarks>
    class ViewportState {
    public:

        /// <summary>
        /// ViewportStateを初期化する
        /// </summary>
        /// <param name="width">描画領域の横幅</param>
        /// <param name="height">描画領域の縦幅</param>
        void Initialize(float width, float height);


        /// <summary>
        /// コマンドリストへViewportとScissorRectを設定する
        /// </summary>
        /// <param name="commandList">描画コマンドを記録するコマンドリスト</param>
        void SetCommand(
            ID3D12GraphicsCommandList *commandList
        );

    private:

        /// ビューポート設定
        D3D12_VIEWPORT viewport_{};

        /// シザー矩形設定
        D3D12_RECT scissorRect_{};
    };
}