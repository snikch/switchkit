#include "Pressing.h"
#include "unity.h"

#ifdef UNIT_TEST

// typedef void (*clickCallback)(void);

void testClickHandler (void) {}

void testFunctionOnClick(void) {
  Pressing testButton(1, HIGH, 400);
  testButton.onClick(10, testClickHandler);
  for (int i = 0; i < 10; i++) {
    TEST_ASSERT_EQUAL(testButton.getClickCallback(i), NULL);
  }
  TEST_ASSERT_EQUAL(testButton.getClickCallback(9), testClickHandler);

  // clickCallback * _clickCallbacks = new clickCallback[1];
  // _clickCallbacks[0] = testClickHandler;
  // int size = 1;
  // int count = 10;
  // int index = count - 1;
  // // Don't allow setting of callback out of bounds.
  // if (size < count) {
  //   clickCallback* temp = new clickCallback[count];
  //   for (int i = 0; i < size; i++) {
  //       temp[i] = _clickCallbacks[i];
  //   }
  //   for (int i = size; i < count; i++) {
  //     temp[i] = NULL;
  //   }
  //   delete [] _clickCallbacks;
  //   _clickCallbacks = temp;
  //   // Serial.printf("Pressing: Did increase array size\n");
  //   size = count;
  // }
  // // Serial.printf("Pressing: click callbacks %d\n", j);
  //   _clickCallbacks[index] = testClickHandler;
  // for (int i = 0; i < size; i++) {
  //     // Serial.printf("Pressing: click callback %d: %p\n", i, _clickCallbacks[i]);
  //   if (i == 0 || i == 9) {
  //     TEST_ASSERT_EQUAL(_clickCallbacks[i], testClickHandler);
  //   } else {
  //     TEST_ASSERT_EQUAL(_clickCallbacks[i], NULL);
  //   }
  // }
  // TEST_ASSERT_EQUAL(10, size);
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(testFunctionOnClick);
    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    process();
}

void loop() {
    // digitalWrite(13, HIGH);
    delay(100);
    // digitalWrite(13, LOW);
    // delay(500);
}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif

#endif
