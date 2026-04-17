struct VertexSharderOutput {
	float32_t4 position : SV_POSITION;
};

struct VertexSharderInput {
    float32_t4  position : POSITION0;
};

VertexSharderOutput main(VertexSharderInput input) {
    VertexSharderOutput output;
    output.position = input.position;
    return output;
}