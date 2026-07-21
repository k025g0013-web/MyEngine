#include "CommandContext.h"
#include "DirectXDevice.h"

#include <cassert>

namespace Kizuna {
	void CommandContext::Initialize(DirectXDevice *device) {
		device_ = device;

		// GPUへ命令を送信するためのCommandQueueを生成
		CreateCommandQueue();

		// CommandListが使用するコマンド記録領域を生成
		CreateCommandAllocator();

		// 実際の描画命令を記録するCommandListを生成
		CreateCommandList();
	}

	void CommandContext::CreateCommandQueue() {
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

		// DirectタイプのCommandQueueを生成する
		// 描画処理やリソース操作など一般的なGPU処理に使用する
		HRESULT hr = device_->GetDevice()->CreateCommandQueue(
			&commandQueueDesc,
			IID_PPV_ARGS(&commandQueue_)
		);

		// CommandQueue生成失敗時は描画処理を続行できないため終了する
		assert(SUCCEEDED(hr));
		(void)hr;
	}

	void CommandContext::CreateCommandAllocator() {
		HRESULT hr = device_->GetDevice()->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator_)
		);

		// CommandAllocator生成失敗時はコマンド記録ができないため終了する
		assert(SUCCEEDED(hr));
		(void)hr;
	}

	void CommandContext::CreateCommandList() {
		HRESULT hr = device_->GetDevice()->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator_.Get(),
			nullptr,
			IID_PPV_ARGS(&commandList_)
		);

		// CommandList生成失敗時は描画処理を行えないため終了する
		assert(SUCCEEDED(hr));
		(void)hr;
	}

	void CommandContext::Close() {
		HRESULT hr = commandList_->Close();

		assert(SUCCEEDED(hr));
		(void)hr;
	}

	void CommandContext::Execute() {
		ID3D12CommandList *commandLists[] = {
			commandList_.Get()
		};

		commandQueue_->ExecuteCommandLists(
			1,
			commandLists
		);
	}

	void CommandContext::Reset() {
		// コマンド記録領域をリセット
		HRESULT hr = commandAllocator_->Reset();
		assert(SUCCEEDED(hr));

		// CommandListを新しい記録状態へ戻す
		hr = commandList_->Reset(
			commandAllocator_.Get(),
			nullptr
		);

		assert(SUCCEEDED(hr));
	}
}