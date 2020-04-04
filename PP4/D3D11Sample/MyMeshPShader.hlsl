Texture2D firstTexture : register(t0);
TextureCube SkyBox : register(t1);

cbuffer Lights : register(b0)
{
	float4 dlColor;
	float4 dlightDir;

	float4 plColor;
	float4 plightPos;

	float4 slColor;
	float4 slightPos;
	float4 coneDir;
};

//static float4 surfaceColor = (1.0f, 1.0f, 1.0f, 1.0f);

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
};

float4 Ambient() : SV_Target
{
    float4 aColor = { 1, 1, 1, 0.3 };
    aColor *= 0.2;
    return aColor;
}

float4 directionalLight(OutputVertex Input) : SV_Target
{
    float4 lRatio = saturate(dot(normalize(-dlightDir), normalize(Input.nrm)));
    
    return (lRatio * dlColor) + Ambient();
}

float4 positionLight(OutputVertex Input) : SV_Target
{
    float3 lightDirection = plightPos - Input.wPos;
    float3 fDist = length(lightDirection);
    lightDirection /= fDist;
    float3 lightRatio = saturate(dot(lightDirection,  Input.nrm));
    float3 fRangeAtt = 1 - saturate(fDist / 10);
    fRangeAtt *= fRangeAtt;
    
    return float4(lightRatio, 0) * float4(fRangeAtt, 0) * plColor;
}

float4 spotLight(OutputVertex Input) : SV_TARGET
{
    float3 lightDirection = normalize(slightPos - Input.wPos);
    float3 surfaceRatio = saturate(dot(-lightDirection, coneDir));
    float3 lightRatio = saturate(dot(lightDirection, Input.nrm));
    float3 att = 1 - saturate((.3 - surfaceRatio) / (.3 - .2));
    att *= att;
    return float4(att, 0) * float4(lightRatio, 0) * slColor;
}

float4 main(OutputVertex Input) : SV_TARGET
{
    return spotLight(Input) + positionLight(Input) + directionalLight(Input) * firstTexture.Sample(MeshTextureSampler, Input.uvw.xy);
}
