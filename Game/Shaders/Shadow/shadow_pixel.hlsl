
struct PS_INPUT{
    float4 position : SV_Position;
};

struct MaterialData{
    float4 albedo;
};

ConstantBuffer<MaterialData> globalMaterialData : register(b2);


float4 main(PS_INPUT input) : SV_Target{

    return globalMaterialData.albedo;

}