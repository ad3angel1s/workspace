#ifndef _STATE_H_
#define _STATE_H_

typedef struct state
{
  int value;
} state_t;

state_t *state_new();
int    state_set(state_t *, int);
int    state_get(state_t *);
void   state_free(state_t *);

#endif /* STATE_H_ */
