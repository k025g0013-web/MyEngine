#include "ImGuiManager.h"

#ifdef USE_IMGUI

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Kizuna {
	ImGuiManager *ImGuiManager::GetInstance() {
		static ImGuiManager instance;
		return &instance;
	}

	void ImGuiManager::Initialize(
		HWND hwnd, ID3D12Device *device, int bufferCount,
		DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap *srvHeap,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
	) {
		// ImGui本体の初期化
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// デフォルトテーマを設定
		ImGui::StyleColorsDark();

		// Win32バックエンドを初期化
		ImGui_ImplWin32_Init(hwnd);

		// DirectX12バックエンドを初期化
		ImGui_ImplDX12_Init(
			device,
			bufferCount,
			rtvFormat,
			srvHeap,
			cpuHandle,
			gpuHandle
		);

		// フォントデータを生成
		ImGuiIO &io = ImGui::GetIO();
		io.Fonts->Build();
	}

	void ImGuiManager::BeginFrame() {
		// DirectX12側の描画準備
		ImGui_ImplDX12_NewFrame();

		// Win32側の入力情報を更新
		ImGui_ImplWin32_NewFrame();

		// GUI構築開始
		ImGui::NewFrame();
	}

	void ImGuiManager::EndFrame() {
		// GUIから描画データを生成
		ImGui::Render();
	}

	void ImGuiManager::Draw(ID3D12GraphicsCommandList *commandList) {
		// EndFrameで生成した描画データをGPUへ送る
		ImGui_ImplDX12_RenderDrawData(
			ImGui::GetDrawData(),
			commandList
		);
	}

	void ImGuiManager::Finalize() {
		// DirectX12バックエンドを終了
		ImGui_ImplDX12_Shutdown();

		// Win32バックエンドを終了
		ImGui_ImplWin32_Shutdown();

		// ImGui本体を破棄
		ImGui::DestroyContext();
	}

#endif
}