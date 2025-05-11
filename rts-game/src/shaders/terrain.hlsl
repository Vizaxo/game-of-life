struct TerrainVert {
	float3 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VSOut {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

VSOut main_vs(in TerrainVert vert) {
	VSOut ret;
	ret.pos = float4(vert.pos, 1.0f);
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in VSOut data) : SV_Target {
	return float4(data.uv, 0.f, 1.f);
}