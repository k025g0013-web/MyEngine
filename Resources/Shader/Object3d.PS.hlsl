#include "object3d.hlsli"
    
struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float32_t intensity;
    int32_t lightType;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // UV変換とテクスチャサンプリング
    float32_t4 transformedUV =
        mul(
            float32_t4(input.texcoord, 0.0f, 1.0f),
            gMaterial.uvTransform
        );

    float32_t4 textureColor =
        gTexture.Sample(gSampler, transformedUV.xy);

    //========================================
    // 2値抜き
    //========================================

    if (textureColor.a < 0.5f)
    {
        discard;
    }

    float32_t4 baseColor =
        gMaterial.color * textureColor;
    
    if (gMaterial.enableLighting == 0 ||
        gDirectionalLight.lightType == 0)
    {
        output.color = baseColor;
    }
    else
    {
        float32_t3 N = normalize(input.normal);
        float32_t3 L =
            normalize(-gDirectionalLight.direction);

        float32_t NdotL = dot(N, L);

        float32_t diffuseFactor = 1.0f;

        if (gDirectionalLight.lightType == 1)
        {
            diffuseFactor = saturate(NdotL);
        }
        else if (gDirectionalLight.lightType == 2)
        {
            diffuseFactor =
                pow(NdotL * 0.5f + 0.5f, 2.0f);
        }

        output.color.rgb =
            baseColor.rgb *
            gDirectionalLight.color.rgb *
            diffuseFactor *
            gDirectionalLight.intensity;

        output.color.a = baseColor.a;
    }

    return output;
}