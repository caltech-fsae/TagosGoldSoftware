#include "helpers.h"
#include "unity.h"

#define PI 3.14159265

// Set up Mocks, testing envivornment
void setUp(void)
{
}

// Deinit everything
void tearDown(void)
{
}

// TODO document what this tests
void test_total_torque(void) {
    double acc = 0, brake = 1;
    TEST_ASSERT_EQUAL_DOUBLE(total_torque(acc, brake), 0);

    acc = 0;
    brake = 0;
    TEST_ASSERT_EQUAL_DOUBLE(total_torque(acc, brake), 0);

    acc = -1;
    brake = 0;
    TEST_ASSERT_EQUAL_DOUBLE(total_torque(acc, brake), 0);

    acc = 5;
    brake = 0;
    TEST_ASSERT_EQUAL_DOUBLE(total_torque(acc, brake), /* replace this with actual function */ 0);
}

// TODO document what this tests
void test_rpm_to_speed(void) {
    double rpm = 1000, d = 10;
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_DOUBLE(rpm_to_speed(rpm, d), 2 * rpm * d * PI);

    rpm = 1000;
    d = -1;
    TEST_ASSERT_EQUAL_DOUBLE(rpm_to_speed(rpm, d), 0);

    rpm = -1;
    d = 10;
    TEST_ASSERT_EQUAL_DOUBLE(rpm_to_speed(rpm, d), 0);
}

// TODO document what this tests
void test_regen_brake(void) {
    double brake = 0, speed = MIN_REGEN_SPEED + 1;
    TEST_ASSERT_EQUAL_INT(regen_brake(brake, speed), 0);

    brake = .1;
    speed = MIN_REGEN_SPEED - 1;
    TEST_ASSERT_EQUAL_INT(regen_brake(brake, speed), 0);

    brake = .1;
    speed = MIN_REGEN_SPEED + 1;
    TEST_ASSERT_EQUAL_INT(regen_brake(brake, speed), 1);
}

// TODO document what this tests
void test_compute_braking_sanity(void) {
    double brake = 0, speed = 0, acc = 0;
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(compute_braking_sanity(brake, speed, acc), 0);

    brake = 1;
    speed = 1;
    acc = 1;
    TEST_ASSERT_EQUAL_INT(compute_braking_sanity(brake, speed, acc), /* Replace with actual function */ 0);

    brake = 0;
    speed = 0;
    acc = 5;
    TEST_ASSERT_EQUAL_DOUBLE(compute_braking_sanity(brake, speed, acc), /* Replace with actual function */ 0);
}

// TODO document what this tests
void test_compute_acceleration_sanity(void) {
    double acc = 0, brake = 0, speed = 0;
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(compute_acceleration_sanity(acc, speed, brake), 0);

    acc = 5;
    brake = 0;
    speed = 0;
    TEST_ASSERT_EQUAL_INT(compute_acceleration_sanity(acc, speed, brake), 0);

    acc = 1;
    brake = 1;
    speed = 1;
    TEST_ASSERT_EQUAL_DOUBLE(compute_acceleration_sanity(acc, speed, brake), /* Replace with actual function */ 0);
}

// TODO document what this tests
void test_compute_steering_sanity(void) {
    double steer = 0, h_acc = 0;
    TEST_IGNORE();
    TEST_ASSERT_EQUAL_INT(compute_steering_sanity(steer, h_acc), 0);

    steer = -1;
    h_acc = 0;
    TEST_ASSERT_EQUAL_INT(compute_steering_sanity(steer, h_acc), 0);

    steer = 5;
    h_acc = 0;
    TEST_ASSERT_EQUAL_INT(compute_steering_sanity(steer, h_acc), 0);

    steer = 5;
    h_acc = 5;
    TEST_ASSERT_EQUAL_INT(compute_steering_sanity(steer, h_acc), /* Replace with actual function */ 0);
}

// TODO document what this tests
void test_receive_from_thread(void) {
    // Mock the threads and test receiving message in mailbox
    TEST_IGNORE();
}

// TODO document what this tests
void test_send_to_thread(void) {
    // Mock the threads and test sending message in mailbox
    TEST_IGNORE();
}
