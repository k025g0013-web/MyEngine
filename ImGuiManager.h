#pragma once

#ifdef USE_IMGUI

#include <d3d12.h>
#include <imgui/imgui.h>

class ImGuiManager {
public:
	// Singleton
	static ImGuiManager *GetInstance();

	// 初期化
	void Initialize(
		HWND hwnd, ID3D12Device *device, int bufferCount,
		DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap *srvHeap,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
	);

	// フレーム開始
	void BeginFrame();

	// フレーム終了
	void EndFrame();

	// 描画
	void Draw(ID3D12GraphicsCommandList *commandList);

	// 終了
	void Finalize();

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;

	ImGuiManager(const ImGuiManager &) = delete;
	ImGuiManager &operator=(const ImGuiManager &) = delete;
};

#endif