//rule of three
//Three things must match
//1. C++ Vertex Struct
//2. Input Layout
//3. HLSL Vertex Struct

struct InputVertex
{
	float3 xyz : POSITION;
	float3 uvw : TEXCOORD;
	float3 nrm : NORMAL;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION0; //system value
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
    float3 wPos : POSITION0;
    float3 lpos : POSITION1;
};

cbuffer SHADERVARS :register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

OutputVertex main(InputVertex input)
{
	OutputVertex output = (OutputVertex)0;
	output.xyzw = float4(input.xyz, 1);
	output.uvw = input.uvw;
    output.nrm = input.nrm;
    output.lpos = input.xyz;
	//Do math here (shader intrinsics)

	output.xyzw = mul(worldMatrix, output.xyzw);
    output.nrm = mul(worldMatrix, float4(output.nrm, 0));
    output.wPos = output.xyzw.xyz;
	output.xyzw = mul(viewMatrix, output.xyzw);
	output.xyzw = mul(projectionMatrix, output.xyzw);
	//don't do perspective divide

	return output;
}