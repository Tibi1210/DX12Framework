

struct VS_INPUT{
    float3 position : POSITION;
    float4 color : COLOR;
};


struct VS_OUTPUT{
    float4 position : SV_Position;
    float4 color : COLOR;
};

struct PassData{
    float4x4 MATRIX_VP;
};
ConstantBuffer<PassData> globalPassData : register(b0);


VS_OUTPUT main(VS_INPUT input){
    
    VS_OUTPUT output;
    
    output.position = mul(globalPassData.MATRIX_VP, float4(input.position, 1.0f));
    output.color = input.color;
    
    return output;
}

	
