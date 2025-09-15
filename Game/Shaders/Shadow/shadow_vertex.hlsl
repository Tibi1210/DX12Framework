

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};


struct VS_OUTPUT{
    float4 position : SV_Position;

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

struct ObjectData{
    float4x4 transform;
    
};
ConstantBuffer<ObjectData> globalObjectData : register(b1);


VS_OUTPUT main(VS_INPUT input){
    
    VS_OUTPUT output;
    
    float4 positionWS = mul(globalObjectData.transform, float4(input.position, 1.0f));
    
    output.position = mul(globalPassData.MATRIX_VP, positionWS);
    return output;
}

	
