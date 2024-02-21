// Pixel shader input
struct PSInput
{
    float4 pos : SV_Position;
    float4 uv : TEXCOORD;
};

// Pixel shader
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    //return g_texture.Sample(g_sampler, input.uv.xy);
    return float4(0, 1, 0, 1);
}