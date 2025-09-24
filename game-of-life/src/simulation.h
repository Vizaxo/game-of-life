#pragma once

#include "common.h"

//constexpr u32 cells_width = 80;
//constexpr u32 cells_height = 60;
constexpr u32 cells_width = 400;
constexpr u32 cells_height = 300;

struct SimState {
	u32 width = cells_width;
	u32 height = cells_height;
	u32 ticks = 0;
	u32 _pad;
};
extern SimState sim_state;

struct CellData {
	XMFLOAT4 color;
};

struct SimCellData {
	struct Data {
		bool enabled;
	};

	// double-buffer the data
	Data data[2];
};

extern CellData cells[cells_width*cells_height];

void init_sim();
void tick_sim(float dt);
