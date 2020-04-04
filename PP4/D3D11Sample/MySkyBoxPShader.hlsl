TextureCube firstTexture : register(t0);

SamplerState MeshTextureSampler : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct OutputVertex
{
    float4 xyzw : SV_POSITION0; //system value
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
    float3 wPos : POSITION0;
    float3 lpos : POSITION1;
};

float4 main(OutputVertex Input) : SV_TARGET
{
    return firstTexture.Sample(MeshTextureSampler, Input.lpos);
}
