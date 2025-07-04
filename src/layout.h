#pragma once

#include "types.h"

void make_layout(state_t *s);

void tiled(state_t *s, int length);
void vertical(state_t *s, int length);
void horizontal(state_t *s, int length);
void pseudofullscreen(state_t *s, int length);

client_t *next_tiled(state_t *s, client_t *cl);

void client_move_resize(state_t *s, client_t *cl, int x, int y, int width, int height);

void swap_clients(state_t *s, client_t *cl1, client_t *cl2);
