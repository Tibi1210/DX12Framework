

struct VS_INPUT{
    float3 position : POSITION;
    float3 normal : NORMAL;
    //float4 color : COLOR;
};


struct VS_OUTPUT{
    float4 position : SV_Position;
    float3 normalWS : NORMAL0;
    float3 normalSS : NORMAL1;
};

struct LightData{

    float3 position;
    float strength;
    float3 direction;
    float _padding;

};

struct PassData{
    float4x4 MATRIX_VP;
    LightData light;
    
};
ConstantBuffer<PassData> globalPassData : register(b0);


VS_OUTPUT main(VS_INPUT input){
    
    VS_OUTPUT output;
    
    output.position = mul(globalPassData.MATRIX_VP, float4(input.position, 1.0f));
    output.normalSS = mul(globalPassData.MATRIX_VP, float4(input.normal, 1.0f)).rgb;
    output.normalWS = input.normal;
    return output;
}

	
