#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t style;
    float32_t3 padding;
    float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    switch (gMaterial.style) {
        case 0:
            {
                output.color = gMaterial.color;
                break;
            }
        case 1:
            {
                float32_t size = 10.0f;
                float32_t2 uv = frac(input.position.xy / size);
                float32_t mask = step(uv.x, 0.5f) * step(uv.y, 0.5f);

                output.color = float32_t4(
                    gMaterial.color.rgb, gMaterial.color.a * mask);
                break;
            }
        case 2:
            {
                float32_t size = 8.0f;
                float32_t mask = step(0.5f, frac(input.position.x / size));

                output.color = float32_t4(gMaterial.color.rgb, gMaterial.color.a * mask);
                break;
            }
    }
    
    return output;
}