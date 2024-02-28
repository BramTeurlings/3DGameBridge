// Pixel shader input
struct PSInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

// Pixel shader
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
//uint enable_alpha : register(b0, space0);

float4 main(PSInput input) : SV_TARGET
{
    float4 layer_color = g_texture.Sample(g_sampler, input.uv.xy);

    //if (ConvertToLinear > 0.5f)
    //{
    //    layer_color = pow(abs(layer_color), 1.0f / 2.333f);
    //}

    // TODO optimize use of alpha if possible

    //bool enable_alpha = true;
    //if (enable_alpha)
    //{
    //    layer_color = float4(layer_color.rgb * layer_color.a, layer_color.a);
    //}

    return layer_color; //float4(input.uv.x, input.uv.y, 0, 1);
}