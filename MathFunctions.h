#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

// 単位行列の作成
inline Matrix4x4 MakeIdentity4x4() {
    Matrix4x4 result = {};
    for (int i = 0; i < 4; i++) {
        result.m[i][i] = 1;
    }
    return result;
}

// 行列の積
inline Matrix4x4 Multiply(const Matrix4x4 &m1, const Matrix4x4 &m2) {
    Matrix4x4 result = {};
    for (int row = 0; row < 4; row++) {
        for (int column = 0; column < 4; column++) {
            for (int k = 0; k < 4; k++) {
                result.m[row][column] += m1.m[row][k] * m2.m[k][column];
            }
        }
    }
    return result;
}

// 行列式
inline float Det3(
    float a1, float a2, float a3,
    float b1, float b2, float b3,
    float c1, float c2, float c3) {

    return
        a1 * (b2 * c3 - b3 * c2) -
        a2 * (b1 * c3 - b3 * c1) +
        a3 * (b1 * c2 - b2 * c1);
}

// 逆行列
inline Matrix4x4 Inverse(const Matrix4x4 &m) {
    Matrix4x4 result = {};

    // 余因子行列
    Matrix4x4 cofactor = {};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float sub[3][3]{};
            int r = 0;

            // 元の行列を走査
            for (int row = 0; row < 4; ++row) {
                if (row == i) continue;
                int c = 0;

                for (int col = 0; col < 4; ++col) {
                    if (col == j) continue;
                    sub[r][c] = m.m[row][col];
                    c++;
                }
                r++;
            }

            // 3x3行列式
            float minor = Det3(
                sub[0][0], sub[0][1], sub[0][2],
                sub[1][0], sub[1][1], sub[1][2],
                sub[2][0], sub[2][1], sub[2][2]
            );

            // 符号付き余因子
            float sign;
            if ((i + j) % 2 == 0) {
                sign = 1.0f;
            } else {
                sign = -1.0f;
            }

            cofactor.m[i][j] = sign * minor;
        }
    }

    // 転置行列
    Matrix4x4 adjugate = {};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            adjugate.m[i][j] = cofactor.m[j][i];
        }
    }

    // 行列式
    float det = 0.0f;
    for (int j = 0; j < 4; ++j) {
        det += m.m[0][j] * cofactor.m[0][j];
    }

    // ゼロチェック
    if (det == 0.0f) {
        return result;
    }

    // 逆行列
    float invDet = 1.0f / det;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = adjugate.m[i][j] * invDet;
        }
    }

    return result;
}

// 平行移動行列
inline Matrix4x4 MakeTranslateMatrix(const Vector3 &translate) {
    Matrix4x4 result = MakeIdentity4x4();

    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;

    return result;
}

// 拡大縮小行列
inline Matrix4x4 MakeScaleMatrix(const Vector3 &scale) {
    Matrix4x4 result = MakeIdentity4x4();

    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;

    return result;
}

// x軸回転行列
inline Matrix4x4 MakeRotateXMatrix(float radian) {
    Matrix4x4 result = MakeIdentity4x4();

    result.m[1][1] = std::cos(radian);
    result.m[1][2] = std::sin(radian);

    result.m[2][1] = -std::sin(radian);
    result.m[2][2] = std::cos(radian);

    return result;
};

// y軸回転行列
inline Matrix4x4 MakeRotateYMatrix(float radian) {
    Matrix4x4 result = MakeIdentity4x4();

    result.m[0][0] = std::cos(radian);
    result.m[0][2] = -std::sin(radian);

    result.m[2][0] = std::sin(radian);
    result.m[2][2] = std::cos(radian);

    return result;
};

// z軸回転行列
inline Matrix4x4 MakeRotateZMatrix(float radian) {
    Matrix4x4 result = MakeIdentity4x4();

    result.m[0][0] = std::cos(radian);
    result.m[0][1] = std::sin(radian);

    result.m[1][0] = -std::sin(radian);
    result.m[1][1] = std::cos(radian);

    return result;
};

// アフィン変換
inline Matrix4x4 MakeAffineMatrix(const Vector3 &scale, const Vector3 &rotation, const Vector3 &translation) {

    Matrix4x4 s = MakeScaleMatrix(scale);

    Matrix4x4 r = Multiply(Multiply(
        MakeRotateXMatrix(rotation.x),
        MakeRotateYMatrix(rotation.y)),
        MakeRotateZMatrix(rotation.z)
    );

    Matrix4x4 t = MakeTranslateMatrix(translation);

    return Multiply(Multiply(s, r), t);
}

// 透視投影行列
inline Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
    Matrix4x4 result = {};

    float fov = 1.0f / std::tan(fovY / 2.0f);

    result.m[0][0] = fov / aspectRatio;
    result.m[1][1] = fov;
    result.m[2][2] = farClip / (farClip - nearClip);
    result.m[2][3] = 1.0f;
    result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

    return result;
};

// 正射影行列
inline Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
    Matrix4x4 result = {};

    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = 1.0f / (farClip - nearClip);
    result.m[3][3] = 1.0f;

    result.m[3][0] = -(right + left) / (right - left);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[3][2] = -nearClip / (farClip - nearClip);

    return result;
};

// 長さ
inline float Length(const Vector3 &v) {
    return sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2));
}

// 正規化
inline Vector3 Normalize(const Vector3 &v) {
    float length = Length(v);
    Vector3 result = {
        v.x / length,
        v.y / length,
        v.z / length,
    };

    return result;
}