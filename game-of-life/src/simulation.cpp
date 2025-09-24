#include "simulation.h"

#include "debug.h"

SimCellData sim_cells[cells_width*cells_height];
CellData cells[cells_width*cells_height];
SimState sim_state;

int mod(int a, int b) {
	return (a%b+b)%b;
}

SimCellData::Data& get_cell(int x, int y, bool current_frame) {
	return sim_cells[(mod(y, cells_height))*cells_width + (mod(x, cells_width))].data[mod(sim_state.ticks + (current_frame ? 0 : 1), 2)];
}

void render_colours() {
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_height; ++j) {
			cells[j*cells_width + i].color.x = get_cell(i, j, true).enabled ? 1.0f : 0.0f;
			cells[j*cells_width + i].color.y = cells[j*cells_width + i].color.z = cells[j*cells_width + i].color.x;
		}
	}
}

void init_sim() {
	// zero
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_height; ++j) {
			get_cell(i, j, true).enabled = false;
			get_cell(i, j, false).enabled = false;
		}
	}

	// glider
	get_cell(-37, 5, true).enabled = true;
	get_cell(-37, 6, true).enabled = true;
	get_cell(-37, 7, true).enabled = true;
	get_cell(-36, 5, true).enabled = true;
	get_cell(-35, 6, true).enabled = true;

	get_cell(306, 250, true).enabled = true;
	get_cell(306, 251, true).enabled = true;
	get_cell(306, 252, true).enabled = true;
	render_colours();
}

void do_tick() {
	for (int i = 0; i < cells_width; ++i) {
		for (int j = 0; j < cells_height; ++j) {
			int neighbours = 0;
			for (int ii = -1; ii < 2; ++ii) {
				for (int jj = -1; jj < 2; ++jj) {
					if ((ii != 0 || jj != 0) && get_cell(i + ii, j + jj, false).enabled)
						++neighbours;
				}
			}

			if (get_cell(i, j, false).enabled)
				get_cell(i, j, true).enabled = neighbours == 2 || neighbours == 3;
			else
				get_cell(i, j, true).enabled = neighbours == 3;
		}
	}
}

void tick_sim(float dt) {
	static float acc_time = 0.;
	acc_time += dt;

	static bool started = false;
	if (!started && acc_time < 5.f)
		return;
	started = true;

	const float tick_delay = 0.003f;
	if (acc_time > tick_delay) {
		acc_time = acc_time - tick_delay;
		++sim_state.ticks;
		do_tick();
		render_colours();
	}
}