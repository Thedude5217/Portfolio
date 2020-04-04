//rule of three
//Three things must match
//1. C++ Vertex Struct
//2. Input Layout
//3. HLSL Vertex Struct

//#pragma pack_matrix(row_major)

static int count = 0;

static int check = 0;

struct InputVertex
{
	float4 xyzw : POSITION;
	float4 rgba : COLOR;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION; //system value
	float4 rgba : OCOLOR;
};

struct MyVertex
{
	float4 xyzw;
	float4 rgba;
};

struct MultDraw
{
	MyVertex Info[12];
};

cbuffer SHADERVARS : register(b0)
{
	float4x4 worldMatrix[2];
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

cbuffer InstanceData : register(b1)
{
	static MultDraw pyramids[2];
	unsigned int size;
};

OutputVertex main(InputVertex input, unsigned int id : SV_InstanceID)
{
	OutputVertex output = (OutputVertex)0;
	output.xyzw = input.xyzw;
	output.rgba = input.rgba;
	//Do math here (shader intrinsics)

	output.xyzw = mul(worldMatrix[id], output.xyzw);

	output.xyzw = mul(viewMatrix, output.xyzw);
	output.xyzw = mul(projectionMatrix, output.xyzw);
	//don't do perspective divide
	return output;
}