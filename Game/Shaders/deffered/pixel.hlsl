
struct VS_OUTPUT{
    float4 positionCS : SV_Position;
    float4 positionWS : POSITION0;
    float3 normalWS : NORMAL0;
    float3 normalSS : NORMAL1;
};

struct PS_OUTPUT{
    float4 positionWS : SV_Target0;
    float4 normalWS : SV_Target1;
    float4 albedo : SV_Target2;
};

struct MaterialData{
    float4 albedo;
};
ConstantBuffer<MaterialData> globalMaterialData : register(b2);

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    output.positionWS = input.positionWS;
    output.normalWS = float4(input.normalWS, 1.0f);
    output.albedo = globalMaterialData.albedo;
    
    return output;
}