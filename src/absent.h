#pragma once

#include "types.h"

state_t *setup();
xcb_atom_t get_atom(state_t *s, char *name);
void setup_atoms(state_t *s);
void clean(state_t *s);
int main();
