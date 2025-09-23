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

struct CellData {
	float4 colour;
};

StructuredBuffer<CellData> cells;

cbuffer sim_state : register(b1)
{
	uint width;
	uint height;
}

v2p main_vs(in MeshVert vert) {
	v2p ret;

	float aspect = screen_size.x / screen_size.y;

	ret.pos = mul(mvp, float4((vert.pos - float2(0.5, 0.5)) * screen_size, 0.0, 1.0));
	ret.uv = vert.uv;
	return ret;
}

float4 main_ps(in v2p data) : SV_Target {
	float2 cell_index = data.uv * float2(width, height);
	int2 cell_index_int = floor(cell_index);
	cell_index_int.x = min(cell_index_int.x, width);
	cell_index_int.y = min(cell_index_int.y, height);

	return cells[cell_index_int.x + cell_index_int.y * width].colour;
	return float4(data.uv, 0.0, 1.0);
}