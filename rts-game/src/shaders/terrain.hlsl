struct TerrainVert {
	float3 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VSOut {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D terrain_tex : register(t0);
SamplerState bilinear : register(s0);

VSOut main_vs(in TerrainVert vert) {
	VSOut ret;
	ret.pos = float4(vert.pos, 1.0f);
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in VSOut data) : SV_Target {
	return terrain_tex.Sample(bilinear, data.uv);
}