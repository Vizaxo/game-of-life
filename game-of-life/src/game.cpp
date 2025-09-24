#include "game.h"

#include "simulation.h"

void init_game() {
	init_sim();
}

void tick_game(float dt) {
	tick_sim(dt);
}
