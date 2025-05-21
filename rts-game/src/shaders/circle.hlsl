struct MeshVert {
	float3 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct v2p {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

cbuffer view_state : register(b0)
{
	float4x4 mvp;
};

v2p main_vs(in MeshVert vert) {
	v2p ret;
	ret.pos = mul(mvp, float4(vert.pos, 1.0f));
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in v2p data) : SV_Target {
	float4 color = float4(1.0, 0.0, 0.0, 1.0);
	return color;
}