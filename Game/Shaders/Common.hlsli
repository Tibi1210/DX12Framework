
struct LightData{
    float3 position;
    float strength;
    float3 direction;
    float _padding;
};

struct PassData{
    float4x4 MATRIX_VP;
    float4x4 LIGHT_MATRIX_VP;
    LightData light; 
};
ConstantBuffer<PassData> globalPassData : register(b0);

struct ObjectData{
    float4x4 transform;
};
ConstantBuffer<ObjectData> globalObjectData : register(b1);