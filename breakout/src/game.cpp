#include "game.h"

std::vector<Block> blocks;

void make_block_grid(IntPoint grid_size, IntPoint centre, IntPoint block_size) {
	blocks.reserve(grid_size.x * grid_size.y);
	IntPoint top_left = centre - grid_size * block_size * 0.5;
	for (int i = 0; i < grid_size.x; ++i) {
		for (int j = 0; j < grid_size.y; ++j) {
			Block b;
			b.mesh_instance.position = top_left + block_size * IntPoint{i, j} + block_size * 0.5;
			b.mesh_instance.scale = block_size;
			b.mesh_instance.scale.z = 1.0;
			b.color = DirectX::Colors::AliceBlue;
			blocks.push_back(b);
		}
	}
}

void game_init() {
	make_block_grid({5, 3}, {0,0}, {50, 50});
}
