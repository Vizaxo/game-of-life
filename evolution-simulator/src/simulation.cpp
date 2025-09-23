#include "simulation.h"

CellData cells[cells_width*cells_height];
SimState sim_state;

void init_sim() {
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_width; ++j) {
			cells[j*cells_width + i].color.x = (float)i / (float)cells_width;
			cells[j*cells_width + i].color.y = (float)j / (float)cells_height;
		}
	}
}

void tick_sim(float dt) {
	++sim_state.ticks;
}