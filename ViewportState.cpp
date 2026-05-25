#include "ViewportState.h"

void ViewportState::Initialize(float width, float height) {
	// ビューポート
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = width;
	viewport_.Height = height;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// シザー矩形
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = static_cast<LONG>(width);
	scissorRect_.top = 0;
	scissorRect_.bottom = static_cast<LONG>(height);
}

void ViewportState::SetCommand(ID3D12GraphicsCommandList *commandList) {
	commandList->RSSetViewports(1, &viewport_);			// Viewportを設定
	commandList->RSSetScissorRects(1, &scissorRect_);	// Scissorを設定
}