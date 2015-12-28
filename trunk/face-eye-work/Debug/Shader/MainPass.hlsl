#pragma pack_matrix ( row_major )

cbuffer CameraData : register(b0)
{
	float4x4 g_mModel;
	float4x4 g_mView;
	float4x4 g_mMV;
	float4x4 g_mMVP;
}

struct DirectionalLight
{
	float3 direction;
	float  intensity;
	float3 color;
	float  specIntensity;
};

#define MAX_N_LIGHTS 6
cbuffer LightData : register(b1)
{
	uint			   g_LightNum : packoffset(c0.x);
	float			   g_Bumpiness : packoffset(c0.y);
	DirectionalLight   g_LightData[MAX_N_LIGHTS] : packoffset(c1);
} 

struct VSInputPosNormUVTan
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vTangent		: TANGENT;
};

struct PSInputPosUVTBN
{ 
	float4 vPosition	: SV_POSITION;
	float2 vTexcoord	: TEXCOORD0;
	float3 vTangent		: TANGENT;
	float3 vBinormal	: BINORMAL;
	float3 vNormal		: NORMAL;
	float3 vViewPosition: POSITIONI0;
};

struct SurfaceData
{
	float3 position;
	float  gloss;
	float3 normal;
	float  specIntensity;
	float3 color;
};

SamplerState g_SamplerLinear	: register(s0);
SamplerState g_SamplerAnisotropic: register(s1);

Texture2D g_TexDiffuseMap : register(t0);
Texture2D g_TexNormalMap : register(t1);
//Texture2D g_

PSInputPosUVTBN MainPassVS(VSInputPosNormUVTan i)//, uint vid : SV_VertexID)
{
	PSInputPosUVTBN o;
	float3 T = mul(float4(i.vTangent.xyz, 0), g_mMV).xyz;
	float3 N = mul(float4(i.vNormal, 0), g_mMV).xyz;
	float3 B = cross(N, T) * i.vTangent.w;

	o.vPosition = mul(float4(i.vPosition, 1), g_mMVP);
	o.vTexcoord = i.vTexcoord;
	o.vTangent = T;
	o.vBinormal = B;
	o.vNormal = N;

	o.vViewPosition = mul(float4(i.vPosition, 1), g_mMV).xyz;
	return o;
}

float BlinnPhongSpecular(float3 n, float3 l, float3 v, float gloss)
{
	float3 h = normalize(l + v);
	float ndh = max(dot(n, h), 0);
	return pow(ndh, gloss);
}

float3 DirectionalLighting(SurfaceData s, DirectionalLight l, float3 v)
{
	float3 mixColor = s.color * l.color;
	float3 diffuse = max(dot(s.normal, l.direction), 0) * l.intensity * mixColor;
	float3 specular = BlinnPhongSpecular(s.normal, l.direction, v, s.gloss) * l.specIntensity * s.specIntensity * mixColor;
	return diffuse + specular;
}

float3 GetPositionFromScreenPixel(float2 screenPos, float linearDepth)
{
	
}

float3 UnpackNormalMap(PSInputPosUVTBN i, Texture2D tex)
{
	float2 vbias = tex.Sample(g_SamplerAnisotropic, i.vTexcoord).ag;
	float2 d = vbias * g_Bumpiness;
	return normalize(normalize(i.vNormal) + d.x * normalize(i.vTangent) + d.y * normalize(i.vBinormal));
}

SurfaceData CreateNormalMapSurfaceData(PSInputPosUVTBN i)
{
	SurfaceData s;
	s.position = float3(0, 0, 0);
	s.normal = UnpackNormalMap(i, g_TexNormalMap);
	s.specIntensity = 0.2;
	s.color =  g_TexDiffuseMap.Sample(g_SamplerAnisotropic, i.vTexcoord).xyz;
	s.gloss = 100;
	return s;
}

float4 MainPassPS(PSInputPosUVTBN i) : SV_Target
{
	float3 shadeColor = float3(0, 0, 0);
	SurfaceData s = CreateNormalMapSurfaceData(i);
	float3 viewVec = -normalize(i.vViewPosition);
	for (uint i = 0; i < g_LightNum; i++)
	{
		shadeColor += DirectionalLighting(s, g_LightData[i], viewVec);
	}
	return float4(shadeColor, 1);
}
