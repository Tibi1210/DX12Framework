

struct PS_INPUT{
    float4 position : SV_Position;
    float3 normalWS : NORMAL0;
    float3 normalSS : NORMAL1;
};

struct MaterialData{
    float4 albedo;
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
ConstantBuffer<MaterialData> globalMaterialData : register(b2);


float4 main(PS_INPUT input): SV_Target{
    
    float3 lightDir = globalPassData.light.direction * -1;
    float LdotN = dot(lightDir, input.normalWS);

    return globalMaterialData.albedo * clamp(LdotN * globalPassData.light.strength, 0.01f, 1.0f);

}