#pragma once

#ifdef USE_IMGUI

#include <d3d12.h>
#include <imgui/imgui.h>

/// <summary>
/// Dear ImGuiの初期化・描画・終了処理を管理するクラス
/// </summary>
/// <remarks>
/// DirectX12およびWin32向けのImGuiバックエンドを初期化し、
/// 毎フレームのGUI描画処理を統括する。
/// Singletonとして実装され、エンジン全体から共通して利用される。
/// </remarks>
class ImGuiManager {
public:
	/// <summary>
	/// ImGuiManagerのインスタンスを取得する。
	/// </summary>
	/// <returns>Singletonインスタンス</returns>
	static ImGuiManager *GetInstance();

	/// <summary>
	/// ImGuiを初期化する。
	/// </summary>
	/// <param name="hwnd">対象ウィンドウ</param>
	/// <param name="device">DirectX12デバイス</param>
	/// <param name="bufferCount">バックバッファ数</param>
	/// <param name="rtvFormat">RTVフォーマット</param>
	/// <param name="srvHeap">SRVディスクリプタヒープ</param>
	/// <param name="cpuHandle">CPU側ディスクリプタハンドル</param>
	/// <param name="gpuHandle">GPU側ディスクリプタハンドル</param>
	void Initialize(
		HWND hwnd, ID3D12Device *device, int bufferCount,
		DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap *srvHeap,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
	);

	/// <summary>
	/// ImGuiのフレームを開始する。
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// ImGuiの描画データを生成する。
	/// </summary>
	void EndFrame();

	/// <summary>
	/// ImGuiを描画する。
	/// </summary>
	/// <param name="commandList">描画に使用するコマンドリスト</param>
	void Draw(ID3D12GraphicsCommandList *commandList);

	/// <summary>
	/// ImGuiを終了し、関連リソースを解放する。
	/// </summary>
	void Finalize();

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;

	ImGuiManager(const ImGuiManager &) = delete;
	ImGuiManager &operator=(const ImGuiManager &) = delete;
};

#endif