#ifndef ABSENT_ABSENT_H
#define ABSENT_ABSENT_H

#include "types.h"

void setup(state_t *s);
xcb_atom_t get_atom(state_t *s, char *name);
void setup_atoms(state_t *s);
void clean(state_t *s);
int main();

#endif
