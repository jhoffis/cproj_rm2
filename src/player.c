#include "player.h"
#include "tester.h"
#include "allocator.h"

void init_player(void) {
    void *test = xmalloc(123);
    void *test2 = xaligned_alloc(123, 16);
    xfree(test);
    xfree(test2);
}

TEST(test_add_positiv2) {
    ASSERT(2 == 2, "Subtraction should work correctly");
}

TEST(test_add_positiv221) {
    ASSERT(2 == 2, "Sasdubtraction should work correctly");
}
