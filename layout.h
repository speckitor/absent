#ifndef ABSENT_LAYOUT_H
#define ABSENT_LAYOUT_H

#include "types.h"

void make_layout(state_t *s);

void main_tiled(state_t *s, int length);
void vertical(state_t *s, int length);
void horizontal(state_t *s, int length);

client_t *next_tiled(client_t *cl);

void client_move_resize(state_t *s, client_t *cl, int x, int y, int width,
                        int height);

#endif
