#include <stdlib.h>

#include "state.h"

state_t *state_new()
{
  state_t *s;

  s = malloc(sizeof(*s));

  if (!s)
    return NULL;

  s->value = 0;

  return s;
}

int state_set(state_t *s, int value)
{
  if (value < 1 || value > 10)
    return -1;

  s->value = value;

  return 0;
}

int state_get(state_t *s)
{
  return s->value;
}

void state_free(state_t *s)
{
  free(s);
}
