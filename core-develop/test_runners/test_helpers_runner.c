/* AUTOGENERATED FILE. DO NOT EDIT. */

//=======Test Runner Used To Run Each Test Below=====
#define RUN_TEST(TestFunc, TestLineNum) \
{ \
  Unity.CurrentTestName = #TestFunc; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(); \
  } \
  if (TEST_PROTECT() && !TEST_IS_IGNORED) \
  { \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

//=======Automagically Detected Files To Include=====
#include "unity.h"
#include <setjmp.h>
#include <stdio.h>
#include "helpers.h"

//=======External Functions This Runner Calls=====
extern void setUp(void);
extern void tearDown(void);
extern void test_total_torque(void);
extern void test_rpm_to_speed(void);
extern void test_regen_brake(void);
extern void test_compute_braking_sanity(void);
extern void test_compute_acceleration_sanity(void);
extern void test_compute_steering_sanity(void);
extern void test_receive_from_thread(void);
extern void test_send_to_thread(void);


//=======Test Reset Option=====
void resetTest(void);
void resetTest(void)
{
  tearDown();
  setUp();
}


//=======MAIN=====
int main(void)
{
  UnityBegin("tests\test_helpers.c");
  RUN_TEST(test_total_torque, 17);
  RUN_TEST(test_rpm_to_speed, 35);
  RUN_TEST(test_regen_brake, 50);
  RUN_TEST(test_compute_braking_sanity, 64);
  RUN_TEST(test_compute_acceleration_sanity, 81);
  RUN_TEST(test_compute_steering_sanity, 98);
  RUN_TEST(test_receive_from_thread, 117);
  RUN_TEST(test_send_to_thread, 123);

  return (UnityEnd());
}