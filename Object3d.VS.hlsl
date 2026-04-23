struct TransformationMatrix {
    float32_t4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexSharderOutput {
	float32_t4 position : SV_POSITION;
};

struct VertexSharderInput {
    float32_t4  position : POSITION0;
};

VertexSharderOutput main(VertexSharderInput input) {
    VertexSharderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    return output;
}