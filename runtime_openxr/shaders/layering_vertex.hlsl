static float4 vertices[3] =
{
    { -1.0f,  3.0f, 0, 1 }, // top-left
    { -1.0f, -1.0f, 0, 1 }, // bottom-left
    { 3.0f,  -1.0f, 0, 1 }  // bottom-right
};

// Draw the texture square in the lower left 'half' of the triangle.
// Since the upper left and lower right vertices are outside the screen, the texture has to be 'pushed' inwards from the top and the right.
// So wrapping the texture at the top and right sides we move it to the visible area of the triangle.
static float2 uvcoords[3] =
{
    { 0.0f, -1.0f},
    { 0.0f, 1.0f},
    { 2.0f, 1.0f}
};

// Pixel shader input
struct PSInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

// Vertex shader
PSInput main(uint VertexIndex : SV_VertexID)
{
    PSInput result;

    result.pos = vertices[VertexIndex];
    result.uv = uvcoords[VertexIndex];

    return result;
}
