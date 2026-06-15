#include "CommandContext.h"
#include "DirectXDevice.h"

#include <cassert>

void CommandContext::Initialize(DirectXDevice* device) {
	device_ = device;

	// コマンドキュー生成
	CreateCommandQueue();

	// コマンドアロケータ生成
	CreateCommandAllocator();

	// コマンドリストを生成する
	CreateCommandList();
}

// コマンドキュー生成
void CommandContext::CreateCommandQueue() {
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device_->GetDevice()->CreateCommandQueue(
		&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

	// コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
	(void)hr;
}

// コマンドアロケータ生成
void CommandContext::CreateCommandAllocator() {
	HRESULT hr = device_->GetDevice()->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));

	// コマンドアロケータの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
	(void)hr;
}

// コマンドリストを生成する
void CommandContext::CreateCommandList() {
	HRESULT hr = device_->GetDevice()->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));

	// コマンドリストの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
	(void)hr;
}

// コマンドリストの内容を確定させる
void CommandContext::Close() {
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));
	(void)hr;
}

void CommandContext::Execute() {
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
}

// 次のフレーム用のコマンドリストを準備
void CommandContext::Reset() {
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));

	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}