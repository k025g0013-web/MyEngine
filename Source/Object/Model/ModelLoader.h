#pragma once

#include <string>

#include "ModelData.h"

namespace Kizuna {

    /// <summary>
    /// objモデルを読み込むローダークラス
    /// </summary>
    /// <remarks>
    /// Wavefront(.obj)形式のモデルを読み込み、
    /// 描画用のModelDataを生成する。
    /// </remarks>
    class ModelLoader {
    public:

        /// <summary>
        /// objファイルを読み込みモデルデータを生成する
        /// </summary>
        static ModelData LoadObjFile(
            const std::string &modelName
        );
    };
}