#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "state.h"

void test1(void **state)
{
  state_t *s;

  s = state_new();

  assert_non_null(s);

  assert_int_equal(state_get(s), 0);

  state_free(s);
}

void test2(void **state)
{
  state_t *s;
  int e;

  s = state_new();
  assert_non_null(s);

  e = state_set(s,5);
  assert_int_equal(e, 0);
  assert_int_equal(state_get(s), 5);

  e = state_set(s,42);
  assert_int_equal(e, -1);
  assert_int_equal(state_get(s), 5);

  state_free(s);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test1),
    cmocka_unit_test(test2),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
