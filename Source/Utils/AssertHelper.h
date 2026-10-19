#pragma once

#include <Windows.h>

#include <cassert>
#include <format>
#include <stdexcept>
#include <string>

namespace Kizuna {

    /// <summary>
    /// HRESULTを確認し、失敗時に例外を送出する
    /// </summary>
    /// <param name="hr">APIの戻り値</param>
    /// <param name="apiName">API名</param>
    inline void CheckHRESULT(
        HRESULT hr,
        const char *apiName = "DirectX API"
    ) {
        if (FAILED(hr)) {
            throw std::runtime_error(
                std::format(
                    "{} に失敗しました。HRESULT: 0x{:08X}",
                    apiName,
                    static_cast<unsigned long>(hr)
                )
            );
        }
    }

    /// <summary>
    /// 条件を確認し、満たさない場合に例外を送出する
    /// </summary>
    /// <param name="condition">確認する条件</param>
    /// <param name="message">失敗時のメッセージ</param>
    inline void CheckCondition(
        bool condition,
        const char *message
    ) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }

    /// <summary>
    /// 開発中の前提条件を確認する
    /// </summary>
    /// <param name="condition">確認する条件</param>
    inline void AssertCondition(bool condition) {
        assert(condition);
        (void)condition;
    }

}