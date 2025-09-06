#pragma once

#include <vector>

#include "common.h"
#include "mesh.h"

struct Block {
	MeshInstance mesh_instance = MeshInstance(quad_mesh.get(), {0., 0., 0.});
	DirectX::XMVECTORF32 color;
};

extern std::vector<Block> blocks;

void game_init();
