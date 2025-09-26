#pragma once

#include "types.h"

void grab_keys(state_t *s);
state_t *setup();
xcb_atom_t get_atom(state_t *s, char *name);
void setup_atoms(state_t *s);
void clean_config(state_t *s);
void clean(state_t *s);
int main();
