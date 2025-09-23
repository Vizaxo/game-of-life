#include "simulation.h"

SimCellData sim_cells[cells_width*cells_height];
CellData cells[cells_width*cells_height];
SimState sim_state;

void render_colours() {
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_width; ++j) {
			cells[j*cells_width + i].color.x = sim_cells[j*cells_width + i].enabled ? 1.0f : 0.0f;
			cells[j*cells_width + i].color.y = cells[j*cells_width + i].color.z = cells[j*cells_width + i].color.x;
		}
	}
}

void init_sim() {
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_width; ++j) {
			sim_cells[j*cells_width + i].enabled = (i % 2) == (j % 2);
		}
	}
	render_colours();
}

void do_tick() {
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_width; ++j) {
			sim_cells[j*cells_width + i].enabled = !sim_cells[j*cells_width + i].enabled;

		}
	}
}

void tick_sim(float dt) {
	static float acc_time = 0.;
	acc_time += dt;

	const float tick_delay = 0.5f;
	if (acc_time > tick_delay) {
		acc_time = acc_time - tick_delay;
		do_tick();
		render_colours();
		++sim_state.ticks;
	}
}