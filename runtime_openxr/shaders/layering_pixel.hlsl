// Pixel shader input
struct PSInput {
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

// Pixel shader
struct temp
{
    int is_opaque;
    int multiply_alpha;
    float convert_to_linear;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
ConstantBuffer<temp> settings : register(b0, space0);

float4 main(PSInput input) : SV_TARGET
{
    float4 layer_color = g_texture.Sample(g_sampler, input.uv.xy);
    layer_color.a = 0.5f;

    // Determine whether we need gamma correction
    layer_color = pow(abs(layer_color), 1.0f / (1.0f + (1.333f * settings.convert_to_linear)));

    // TODO need to test these implementations
    // Pre-multiply alpha or not
    float alpha = 1 - (1 - layer_color.a) * settings.multiply_alpha;
    // Determine whether the layer should be fully opaque or with alpha blending
    float blend = layer_color.a + (1 - layer_color.a) * settings.is_opaque;
    layer_color = float4(layer_color.rgb * alpha, blend);

    return layer_color;
}