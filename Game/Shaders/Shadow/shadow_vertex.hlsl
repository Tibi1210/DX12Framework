#include "../Common.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT{
    float4 position : SV_Position;

};

VS_OUTPUT main(VS_INPUT input){
    
    VS_OUTPUT output;
    
    float4 positionWS = mul(globalObjectData.transform, float4(input.position, 1.0f));
    
    output.position = mul(globalPassData.MATRIX_VP, positionWS);
    return output;
}

	
