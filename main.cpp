#include <windows.h>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	// デバッグテキスト
	OutputDebugStringA("Hello,DirectX!\n");

	return 0;
}