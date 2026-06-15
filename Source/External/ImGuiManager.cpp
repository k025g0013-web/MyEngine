#include "ImGuiManager.h"

#ifdef USE_IMGUI

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImGuiManager *ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(
	HWND hwnd, ID3D12Device *device, int bufferCount,
	DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap *srvHeap,
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		bufferCount,
		rtvFormat,
		srvHeap,
		cpuHandle,
		gpuHandle
	);

	ImGuiIO &io = ImGui::GetIO();
	io.Fonts->Build();
}

void ImGuiManager::BeginFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
	ImGui::Render();
}

void ImGuiManager::Draw(ID3D12GraphicsCommandList *commandList) {
	ImGui_ImplDX12_RenderDrawData(
		ImGui::GetDrawData(),
		commandList
	);
}

void ImGuiManager::Finalize() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

#endif