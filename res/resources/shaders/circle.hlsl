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

	float2 pos = vert.pos * 2.0 - 1.0;
	pos.x /= aspect;
	ret.pos = float4(pos, 0.f, 1.f);
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in v2p data) : SV_Target {
	float4 color = float4(sin(time)/2.0+0.5, cos(time)/2.0+0.5, sin(time / 3.f), 1.0);
	float4 bg = float4(0.f, 0., 0., 0.0);

	float2 uv = data.uv;
	uv *= 2.;
	uv -= 1.;
	float x = uv.x;
	float y = uv.y;

	float h = sqrt(x*x + y*y);
	float theta = sin(uv.x/h);

	float t = time;
	t *= 2.f;

	float r = sin(t + 2.*theta)/2.0 + 0.5;
	r = 0.3 + 0.1*r;
	if (uv.x*uv.x + uv.y*uv.y < r)
		return color;
	else
		return bg;
}