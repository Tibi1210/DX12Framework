

struct VS_INPUT{
    float3 position : POSITION;
    float3 normal : NORMAL;
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

struct ObjectData{
    float4x4 transform;
    
};
ConstantBuffer<ObjectData> globalObjectData : register(b1);


VS_OUTPUT main(VS_INPUT input){
    
    VS_OUTPUT output;
    
    float4 positionWS = mul(globalObjectData.transform, float4(input.position, 1.0f));
    
    output.position = mul(globalPassData.MATRIX_VP, positionWS);
    output.normalSS = mul(globalPassData.MATRIX_VP, float4(input.normal, 1.0f)).rgb;
    output.normalWS = mul(globalObjectData.transform, float4(input.normal, 1.0f)).rgb;
    return output;
}

	
