#include "simulation.h"

CellData cells[cells_width*cells_height];
SimState sim_state;

void init_sim() {
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_width; ++j) {
			XMStoreFloat4(&cells[j*cells_width + i].color, Colors::Green);
		}
	}
}

void tick_sim(float dt) {
	++sim_state.ticks;
}