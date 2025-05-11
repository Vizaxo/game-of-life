struct TerrainVert {
	float3 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct VSOut {
	float4 pos : SV_POSITION;
};

VSOut main_vs(in TerrainVert vert) {
	VSOut ret;
	ret.pos = float4(vert.pos, 1.0f);
	return ret;
}

float4 main_ps(in VSOut data) : SV_Target {
	return data.pos;
}