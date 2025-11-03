#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "types.h"

void make_layout(state_t *s);

void client_move_resize(state_t *s, client_t *cl, int x, int y, int width, int height);

void swap_clients(state_t *s, client_t *cl1, client_t *cl2);

#endif // LAYOUT_H_
