
#include "../../Common.hlsli"

struct VS_OUTPUT
{
    float4 positionCS : SV_Position;
    float2 uv : TEXCOORD0;

};

Texture2D gPositionBuffer : register(t0);
Texture2D gNormalBuffer : register(t1);
Texture2D gAlbedoBuffer : register(t2);
Texture2D gGDepthBuffer : register(t3);
Texture2D gShadowBuffer : register(t4);

SamplerState sPointClamp : register(s0, space0);
SamplerState sLinearClamp : register(s1, space0);

SamplerComparisonState sShadowCompare : register(s0, space1);

float CalcShadowfactor(float4 lightPosCS)
{
    const float4x4 toUV =
    {
        float4(0.5f, 0.0f, 0.0f, 0.0f),
        float4(0.0f, -0.5f, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(0.5f, 0.5f, 0.0f, 1.0f)
    };
    
    float4 uv = mul(lightPosCS, toUV);
    
    unsigned int w, h;
    gShadowBuffer.GetDimensions(w, h);
    
    float delta = 1.0f / (float) w;
    
    const unsigned int kernelSize = 9;
    const float2 kernel[kernelSize] =
    {
        float2(-delta, -delta), float2(0.0f, -delta), float2(delta, -delta),
        float2(-delta, 0.0f), float2(0.0f, 0.0f), float2(delta, 0.0f),
        float2(-delta, delta), float2(0.0f, delta), float2(delta, delta)
    };
    
    float lightAcc = 1.0f;
    
    [unroll]
    for (unsigned int i = 0; i < kernelSize; i++)
    {
        lightAcc += gShadowBuffer.SampleCmpLevelZero(sShadowCompare, uv.xy + kernel[i], uv.z).r;
    }
    
    return lightAcc / (float)kernelSize;
}


float4 main(VS_OUTPUT input) : SV_TARGET
{
    
    float3 position = gPositionBuffer.Sample(sPointClamp, input.uv).xyz;
    float4 color = gAlbedoBuffer.Sample(sPointClamp, input.uv);
    float4 normalWS = gNormalBuffer.Sample(sPointClamp, input.uv);
    float depth = gGDepthBuffer.Sample(sPointClamp, input.uv).r;
    
    float4 lightObjectPosCS = mul(globalPassData.LIGHT_MATRIX_VP, float4(position, 1.0f));
    float shadowFactor = CalcShadowfactor(lightObjectPosCS);
    
    float3 lightDir = normalize(globalPassData.light.direction) * -1;
    float LdotN = dot(lightDir, normalWS.xyz);

    return shadowFactor * color * clamp(LdotN * globalPassData.light.strength, 0.01f, 1.0f);
    
}