struct TerrainVert {
	float3 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct v2p {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D grass : register(t0);
Texture2D water : register(t1);
Texture2D stone : register(t2);
SamplerState bilinear : register(s0);

cbuffer view_state : register(b0)
{
	float4x4 mvp;
};

v2p main_vs(in TerrainVert vert) {
	v2p ret;
	ret.pos = mul(mvp, float4(vert.pos, 1.0f));
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in v2p data) : SV_Target {
	float height = data.pos.z;
	float4 color;
	if (height < 0.2)
		color = water.Sample(bilinear, data.uv);
	else if (height < 0.5)
		color = grass.Sample(bilinear, data.uv);
	else
		color = stone.Sample(bilinear, data.uv);
	return color;
}