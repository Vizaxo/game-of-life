struct MeshVert {
	float2 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct v2p {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

cbuffer view_state : register(b0)
{
	float4x4 mvp;
	float2 screen_size;
	float time;
};

v2p main_vs(in MeshVert vert) {
	v2p ret;

	float aspect = screen_size.x / screen_size.y;

	ret.pos = mul(mvp, float4((vert.pos - float2(0.5, 0.5)) * screen_size, 0.0, 1.0));
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in v2p data) : SV_Target {
	return float4(data.uv, 0.0, 1.0);
}