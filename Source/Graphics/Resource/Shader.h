#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <string>

namespace Kizuna {
    class Logger;

    /// <summary>
    /// HLSLシェーダーのコンパイルを管理するクラス
    /// </summary>
    /// <remarks>
    /// DXCコンパイラを利用してHLSLファイルをコンパイルし、
    /// DirectX12で利用可能なシェーダーバイナリを生成・保持する。
    /// </remarks>
    class Shader {
    public:
        /// <summary>
        /// DXCコンパイラを初期化する
        /// </summary>
        /// <param name="logger">ログ出力クラス</param>
        static void InitializeCompiler(Logger *logger);

        /// <summary>
        /// HLSLファイルをコンパイルする
        /// </summary>
        /// <param name="filePath">シェーダーファイルのパス</param>
        /// <param name="profile">コンパイルプロファイル</param>
        void Compile(const std::wstring &filePath, const wchar_t *profile);

        /// <summary>
        /// コンパイル済みシェーダーBlobを取得する
        /// </summary>
        /// <returns>シェーダーBlob</returns>
        IDxcBlob *GetBlob() const { return shaderBlob_.Get(); }

        /// <summary>
        /// DirectX12で利用するシェーダーバイトコードを取得する
        /// </summary>
        /// <returns>シェーダーバイトコード</returns>
        D3D12_SHADER_BYTECODE GetBytecode() const {
            return { shaderBlob_->GetBufferPointer(), shaderBlob_->GetBufferSize() };
        }

    private:
        /// ログ出力クラス
        static Logger *sLogger_;

        /// DXCユーティリティ
        static Microsoft::WRL::ComPtr<IDxcUtils> sDxcUtils_;

        /// DXCコンパイラ
        static Microsoft::WRL::ComPtr<IDxcCompiler3> sDxcCompiler_;

        /// Includeファイル管理
        static IDxcIncludeHandler *sIncludeHandler_;

        /// コンパイル済みシェーダーバイナリ
        Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob_;
    };
}