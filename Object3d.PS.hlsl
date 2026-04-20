struct Material {
    float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelSharderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelSharderOutput main()
{
    PixelSharderOutput output;
    output.color = gMaterial.color;
    return output;
}