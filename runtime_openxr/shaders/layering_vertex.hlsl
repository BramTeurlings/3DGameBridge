static float4 vertices[4] =
{
    {  1.0f, -1.0f, 0, 1 },
    { -1.0f, -1.0f, 0, 1 },
    {  1.0f,  1.0f, 0, 1 },
    { -1.0f,  1.0f, 0, 1 }
};

// Pixel shader input
struct PSInput
{
    float4 pos : SV_Position;
    float4 uv : TEXCOORD;
};

// Vertex shader
PSInput main(uint VertexIndex : SV_VertexID)
{
    PSInput result;

    result.pos = vertices[VertexIndex];
    result.uv = vertices[VertexIndex];

    return result;
}
