
#include "../Common.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 normalWS : NORMAL0;
    float3 normalSS : NORMAL1;
};


VS_OUTPUT main(VS_INPUT input)
{
    
    VS_OUTPUT output;
    
    float4 positionWS = mul(globalObjectData.transform, float4(input.position, 1.0f));
    
    output.position = mul(globalPassData.LIGHT_MATRIX_VP, positionWS);
    output.normalSS = mul(globalPassData.LIGHT_MATRIX_VP, float4(input.normal, 1.0f)).rgb;
    output.normalWS = mul((float3x3) globalObjectData.transform, input.normal);
    return output;
}

	
