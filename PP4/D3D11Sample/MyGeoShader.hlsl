//rule of three
//Three things must match
//1. C++ Vertex Struct
//2. Input Layout
//3. HLSL Vertex Struct

//#pragma pack_matrix(row_major)


struct OutputVertex
{
    float4 xyzw : SV_POSITION; //system value
    float4 rgba : OCOLOR;
};

cbuffer SHADERVARS :register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

[maxvertexcount(3)]

void main(point float4 input[1] : SV_Position, inout TriangleStream<OutputVertex> output)
{
    OutputVertex verts[3];

    verts[0].rgba = float4(0.1f, 0.6f, 0.1f, 1);
    verts[1].rgba = float4(0, 1, 0, 1);
    verts[2].rgba = verts[0].rgba;
    
    verts[0].xyzw = input[0].xyzw;
    verts[1].xyzw = verts[0].xyzw;
    verts[2].xyzw = verts[0].xyzw;
    
    verts[0].xyzw.x -= 0.3f;
    verts[1].xyzw.y += 1;
    verts[2].xyzw.x += 0.3f;
    
    for (uint i = 0; i < 3; ++i)
    {
        output.Append(verts[i]);
    }
}