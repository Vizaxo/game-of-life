#pragma once

#include "common.h"

constexpr u32 cells_width = 10;
constexpr u32 cells_height = 10;

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
	bool enabled;
};

extern CellData cells[cells_width*cells_height];

void init_sim();
void tick_sim(float dt);
