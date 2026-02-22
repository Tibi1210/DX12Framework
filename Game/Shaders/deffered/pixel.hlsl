
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
//ConstantBuffer<MaterialData> globalMaterialData : register(b2);

cbuffer MaterialBuffer : register(b2)
{
    MaterialData materials[6];
}

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    output.positionWS = input.positionWS;
    output.normalWS = float4(input.normalWS, 1.0f);
    //output.albedo = globalMaterialData.albedo;
    // there is no [1] material for floor thats why it is black...
    output.albedo = materials[0].albedo;
    
    return output;
}