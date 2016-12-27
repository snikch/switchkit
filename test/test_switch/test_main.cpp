#include "switch.h"
#include "unity.h"

#ifdef UNIT_TEST

// Describe: A smart switch
  // Describe: in online mode
    // Describe: set to off
      // It: has the output on
      // Describe: when toggled
        // It: leaves the output on
        // It: submits off to the mqtt topic
  // Describe: in offline mode
    // Describe: set to off


void testFunctionSwitchOffline(void) {
  Switch testSw("switch", 1, 2);
  testButton.onClick(10, testClickHandler);
  for (int i = 0; i < 10; i++) {
    TEST_ASSERT_EQUAL(testButton.getClickCallback(i), NULL);
  }
  TEST_ASSERT_EQUAL(testButton.getClickCallback(9), testClickHandler);

}

void process() {
    UNITY_BEGIN();
    RUN_TEST(testFunctionSwitchOffline);
    UNITY_END();
}

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif
