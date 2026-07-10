#pragma once

#include <windows.h>
#include <fstream>
#include <string>

/// <summary>
/// ログ出力とクラッシュダンプの生成を管理するクラス
/// </summary>
/// <remarks>
/// 実行中のログをファイルおよびデバッグ出力へ記録する。
/// また、未処理例外発生時にはMiniDumpファイルを生成し、
/// クラッシュ時の原因解析を支援する。
/// </remarks>
class Logger {
public:
    /// <summary>
    /// ログシステムを初期化する
    /// </summary>
    /// <remarks>
    /// ログファイルを生成し、未処理例外発生時の
    /// クラッシュハンドラを登録する。
    /// </remarks>
    void Initialize();

    /// <summary>
    /// ログシステムを終了する
    /// </summary>
    void Finalize();

    /// <summary>
    /// ログを出力する
    /// </summary>
    /// <param name="message">出力する文字列</param>
    void Log(const std::string &message);

private:

    /// <summary>
    /// 未処理例外発生時にMiniDumpを生成する
    /// </summary>
    /// <param name="exception">例外情報</param>
    /// <returns>例外処理結果</returns>
    static LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception);

private:
    /// ログファイル出力ストリーム
    std::ofstream stream_;
};