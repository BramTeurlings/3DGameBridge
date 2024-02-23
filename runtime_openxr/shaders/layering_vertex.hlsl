static float4 vertices[3] =
{
    { -1.0f,  3.0f, 0, 1 },
    { -1.0f, -1.0f, 0, 1 },
    {  3.0f, -1.0f, 0, 1 }
};
static float2 uvcoords[3] =
{
    { -1.0f,  1.0f},
    { -1.0f, -1.0f},
    {  1.0f, -1.0f}
};

// Pixel shader input
struct PSInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// Vertex shader
PSInput main(uint VertexIndex : SV_VertexID)
{
    PSInput result;

    result.pos = vertices[VertexIndex];
    result.uv = uvcoords[VertexIndex];

    return result;
}
