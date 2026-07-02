#include "Easing.h"
#include "Vector.h"

template <typename T>
T Easing::Interpolate(EasingType type, float t, const T &start, const T &end) {
    t = std::clamp(t, 0.0f, 1.0f);
    float rate = t;

    switch (type) {
    case EasingType::Linear: rate = t; break;
    
    case EasingType::easeInSine:     rate = InSine(t); break;
    case EasingType::easeOutSine:    rate = OutSine(t); break;
    case EasingType::easeInOutSine:  rate = InOutSine(t); break;
    
    case EasingType::easeInQuad:     rate = InQuad(t); break;
    case EasingType::easeOutQuad:    rate = OutQuad(t); break;
    case EasingType::easeInOutQuad:  rate = InOutQuad(t); break;
    
    case EasingType::easeInCubic:    rate = InCubic(t); break;
    case EasingType::easeOutCubic:   rate = OutCubic(t); break;
    case EasingType::easeInOutCubic: rate = InOutCubic(t); break;
    
    case EasingType::easeInQuart:    rate = InQuart(t); break;
    case EasingType::easeOutQuart:   rate = OutQuart(t); break;
    case EasingType::easeInOutQuart: rate = InOutQuart(t); break;
    
    case EasingType::easeInQuint:    rate = InQuint(t); break;
    case EasingType::easeOutQuint:   rate = OutQuint(t); break;
    case EasingType::easeInOutQuint: rate = InOutQuint(t); break;
    
    case EasingType::easeInExpo:     rate = InExpo(t); break;
    case EasingType::easeOutExpo:    rate = OutExpo(t); break;
    case EasingType::easeInOutExpo:  rate = InOutExpo(t); break;
    
    case EasingType::easeInCirc:     rate = InCirc(t); break;
    case EasingType::easeOutCirc:    rate = OutCirc(t); break;
    case EasingType::easeInOutCirc:  rate = InOutCirc(t); break;
    
    case EasingType::easeInBack:     rate = InBack(t); break;
    case EasingType::easeOutBack:    rate = OutBack(t); break;
    case EasingType::easeInOutBack:  rate = InOutBack(t); break;
    
    case EasingType::easeInElastic:  rate = InElastic(t); break;
    case EasingType::easeOutElastic: rate = OutElastic(t); break;
    case EasingType::easeInOutElastic: rate = InOutElastic(t); break;
    
    case EasingType::easeInBounce:   rate = InBounce(t); break;
    case EasingType::easeOutBounce:  rate = OutBounce(t); break;
    case EasingType::easeInOutBounce: rate = InOutBounce(t); break;
    }

    return start + (end - start) * rate;
}

#pragma region イージング関数群
float Easing::InSine(float t) { return 1.0f - std::cos((t * static_cast<float>(M_PI)) / 2.0f); }
float Easing::OutSine(float t) { return std::sin((t * static_cast<float>(M_PI)) / 2.0f); }
float Easing::InOutSine(float t) { return -(std::cos(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f; }

float Easing::InQuad(float t) { return t * t; }
float Easing::OutQuad(float t) { return t * (2.0f - t); }
float Easing::InOutQuad(float t) { return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f; }

float Easing::InCubic(float t) { return t * t * t; }
float Easing::OutCubic(float t) { float invT = 1.0f - t; return 1.0f - (invT * invT * invT); }
float Easing::InOutCubic(float t) { return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f; }

float Easing::InQuart(float t) { float t2 = t * t; return t2 * t2; }
float Easing::OutQuart(float t) { float invT = 1.0f - t; float invT2 = invT * invT; return 1.0f - (invT2 * invT2); }
float Easing::InOutQuart(float t) { return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f; }

float Easing::InQuint(float t) { float t2 = t * t; return t2 * t2 * t; }
float Easing::OutQuint(float t) { return 1.0f - std::pow(1.0f - t, 5.0f); }
float Easing::InOutQuint(float t) { return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f; }

float Easing::InExpo(float t) { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f); }
float Easing::OutExpo(float t) { return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }
float Easing::InOutExpo(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

float Easing::InCirc(float t) { return 1.0f - std::sqrt(1.0f - (t * t)); }
float Easing::OutCirc(float t) { float tMinus1 = t - 1.0f; return std::sqrt(1.0f - (tMinus1 * tMinus1)); }
float Easing::InOutCirc(float t) {
    return t < 0.5f ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f
        : (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

// Back, Elastic, Bounce 用のマジックナンバー
namespace {
    constexpr float C1 = 1.70158f;
    constexpr float C2 = C1 * 1.525f;
    constexpr float C3 = C1 + 1.0f;
    constexpr float C4 = (2.0f * static_cast<float>(M_PI)) / 3.0f;
    constexpr float C5 = (2.0f * static_cast<float>(M_PI)) / 4.5f;
}

float Easing::InBack(float t) { return C3 * t * t * t - C1 * t * t; }
float Easing::OutBack(float t) { float tMinus1 = t - 1.0f; return 1.0f + C3 * tMinus1 * tMinus1 * tMinus1 + C1 * tMinus1 * tMinus1; }
float Easing::InOutBack(float t) {
    return t < 0.5f ? (std::pow(2.0f * t, 2.0f) * ((C2 + 1.0f) * 2.0f * t - C2)) / 2.0f
        : (std::pow(2.0f * t - 2.0f, 2.0f) * ((C2 + 1.0f) * (t * 2.0f - 2.0f) + C2) + 2.0f) / 2.0f;
}

float Easing::InElastic(float t) {
    if (t == 0.0f) return 0.0f; if (t == 1.0f) return 1.0f;
    return -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * C4);
}
float Easing::OutElastic(float t) {
    if (t == 0.0f) return 0.0f; if (t == 1.0f) return 1.0f;
    return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * C4) + 1.0f;
}
float Easing::InOutElastic(float t) {
    if (t == 0.0f) return 0.0f; if (t == 1.0f) return 1.0f;
    return t < 0.5f ? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * C5)) / 2.0f
        : (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * C5)) / 2.0f + 1.0f;
}

float Easing::OutBounce(float t) {
    const float n1 = 7.5625f; const float d1 = 2.75f;
    if (t < 1.0f / d1) { return n1 * t * t; } else if (t < 2.0f / d1) { t -= 1.5f / d1; return n1 * t * t + 0.75f; } else if (t < 2.5f / d1) { t -= 2.25f / d1; return n1 * t * t + 0.9375f; } else { t -= 2.625f / d1; return n1 * t * t + 0.984375f; }
}
float Easing::InBounce(float t) { return 1.0f - OutBounce(1.0f - t); }
float Easing::InOutBounce(float t) {
    return t < 0.5f ? (1.0f - OutBounce(1.0f - 2.0f * t)) / 2.0f : (1.0f + OutBounce(2.0f * t - 1.0f)) / 2.0f;
}
#pragma endregion

// 明示的実体化
template float Easing::Interpolate<float>(EasingType, float, const float &, const float &);
template Vector2 Easing::Interpolate<Vector2>(EasingType, float, const Vector2 &, const Vector2 &);
template Vector3 Easing::Interpolate<Vector3>(EasingType, float, const Vector3 &, const Vector3 &);
template Vector4 Easing::Interpolate<Vector4>(EasingType, float, const Vector4 &, const Vector4 &);