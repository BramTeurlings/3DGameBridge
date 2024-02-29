// Pixel shader input
struct PSInput {
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

// Pixel shader
struct temp
{
    uint is_opaque;
    uint multiply_alpha;
    uint correct_gamma;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
ConstantBuffer<temp> settings : register(b0, space0); // Uses b0 and b1 from space0

float4 main(PSInput input) : SV_TARGET
{
    float4 layer_color = g_texture.Sample(g_sampler, input.uv.xy);

    // Determine whether we need gamma correction
    if (settings.correct_gamma > 0.5f) {
        layer_color = pow(abs(layer_color), 1.0f / 2.333f);
        layer_color.r += 0.5f;
    }

    // Pre-multiply alpha or not
    float alpha = layer_color.a + (1 - layer_color.a) * settings.multiply_alpha;
    // Determine whether the layer should be fully opaque or with alpha blending
    float blend = layer_color.a + (1 - layer_color.a) * settings.is_opaque;
    layer_color = float4(layer_color.rgb * alpha, blend);

    layer_color.g += 0.5 * settings.is_opaque;
    layer_color.b += 0.5 * settings.multiply_alpha;

    return layer_color; //float4(input.uv.x, input.uv.y, 0, 1);
}