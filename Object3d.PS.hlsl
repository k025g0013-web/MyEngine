struct PixelSharderOutput {
    float4 color : SV_TARGET0;
};

PixelSharderOutput main() {
    PixelSharderOutput output;
    output.color = float4(1.0, 1.0, 1.0, 1.0);
    return output;
}