
struct LightData{
    float3 position;
    float strength;
    float3 direction;
    float _padding;
};

struct PassData{

    float4x4 MATRIX_V;
    float4x4 MATRIX_P;
    float4x4 MATRIX_VP;
    float4x4 MATRIX_V_I;
    float4x4 MATRIX_P_I;
    float4x4 MATRIX_VP_I;

    float3 EYE_POS;
    float _padding;
    float2 RENDER_TARGET_SIZE;
    float2 RENDER_TARGET_SIZE_I;
    float NEAR_Z;
    float FAR_Z;
    float TOTAL_TIME;
    float DELTA_TIME;

    float4x4 LIGHT_MATRIX_VP;
    LightData sceneLight;
};
ConstantBuffer<PassData> globalPassData : register(b0);

struct ObjectData{
    float4x4 transform;
};
ConstantBuffer<ObjectData> globalObjectData : register(b1);