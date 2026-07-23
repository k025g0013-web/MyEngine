#include "NormalCamera.h"

namespace Kizuna {
    void NormalCamera::Update(const Transform &transform) {
        // カメラに使うMatrix群の更新
        UpdateMatrices(transform);
    }
}