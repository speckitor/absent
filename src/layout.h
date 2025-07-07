#pragma once

#include "types.h"

void make_layout(state_t *s);

void tiled(state_t *s, int number);
void rtiled(state_t *s, int number);
void vertical(state_t *s, int number);
void horizontal(state_t *s, int number);

client_t *next_tiled(state_t *s, client_t *cl);

void client_move_resize(state_t *s, client_t *cl, int x, int y, int width, int height);

void swap_clients(state_t *s, client_t *cl1, client_t *cl2);
