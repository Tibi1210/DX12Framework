struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 positionCS : SV_Position;
    float2 uv : TEXCOORD0;

};

VS_OUTPUT main(unsigned int idx : SV_VertexID){
    VS_OUTPUT output;
    
    output.uv = float2((idx << 1) & 2, idx & 2);
    output.positionCS = float4(output.uv.x * 2.0f - 1.0f, -output.uv.y * 2.0f + 1.0f, 0.0f, 1.0f);
    
    
    return output;
}