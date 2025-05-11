struct TerrainVert {
	float3 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VSOut {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D grass : register(t0);
Texture2D water : register(t1);
Texture2D stone : register(t2);
SamplerState bilinear : register(s0);

VSOut main_vs(in TerrainVert vert) {
	VSOut ret;
	ret.pos = float4(vert.pos.xy / 10.0 - float2(0.5,0.5), vert.pos.z, 1.0f);
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in VSOut data) : SV_Target {
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