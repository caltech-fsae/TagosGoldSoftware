#include "main.c"
#include "unity.h"

// Set up Mocks, testing envivornment
void setUp(void)
{
}

// Deinit everything
void tearDown(void)
{
}

// This tests that the main motor_control function works.
void test_motor_control(void) {
    TEST_IGNORE();
    // set data_buffer values for all motor_control inputs in Mocks
    // TODO
    // test computed values sent by motor_control
    // TODO
}

// This tests that the heartbeat function detects failures.
void test_heartbeat(void) {
    TEST_IGNORE();
    // set data_buffers such that all heartbeats have been received
    // TODO
    // test that there are no failures detected
    // TODO

    // set data_buffers such that all but one heartbeats have been received
    // TODO
    // test that failure is detected
    // TODO
}

// This tests that the sanity check functions for both checking acceleration
// braking and checking steering works.
void test_sanity_check(void) {
    TEST_IGNORE();
    // Acceleration and Braking Sanity Check
    
    // set data buffers for all input in Mocks.
    //(angle of accelerator, angle of brake pedal, RPM, IMU acceleration,
    // previous sanity check values)
    // TODO
    
    // Test that current speed of vehicle using RPM info is correct.
    // TODO
    
    // Test that the new and old speed values are compared and a sanity 
    // check is made.
    // TODO
    
    // test that OK or error message is sent to CORE
    // TODO
    
    // Steering sanity check
    
    // Set data buffers for all input in mocks
    // Angle of steering wheel, IMU acceleration, previous check info.
    
    // TODO
    
    // Test that the expected horizontal acceleration is computed.
    
    // TODO
    
    // Test that values are compared and ok or error message is sent to
    // CORE.
    
    // TODO
}

// This tests that the driver display function works. Using Unity's CMock
void test_lcd_control(void) {
    
    // Functions such as (WriteDisplay_ExpectAndReturn) requires cmock to use.
    // Run Cmock on header files to generate mocked functions.
    
    // TODO: Data struct should be updated to contain some value.
    data mock_data;
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    
    // set data_buffer values for all car information in with mocked data,
    data_buffer rpm = {1, &mock_data, mut,1};
    data_buffer torque = {1, &mock_data, mut,1};
    data_buffer max_temp = {1, &mock_data, mut, 1};
    data_buffer core_temp = {1, &mock_data, mut, 1};
    data_buffer battery = {1, &mock_data, mut, 1};
	data_buffer limp = {1, &mock_data, mut, 1}
  
    // Create an array the RGB values for the screen. 
    // TODO
    // We should replace screenSize with size of the screen and set data_array
    // equal to some template GUI we will use for the display. 
    // That way we can verify that our function matches the template only with values replaced with
    // data buffers above.
    // TODO
    int screenSize = 0;
    int data_array[screenSize];
	
    TEST_IGNORE();
    
    // Mock the functions that should be called when testing lcd_control.
    // State what values they are expected to take and return.
    WriteDisplay_ExpectAndReturn(&data_array, sizeof(data_array), SUCCESS);
    
    // Run actual lcd_control function with our data.
    lcd_control(rpm, torque, max_temp, core_temp, battery, limp);
    
    // If mocked functions receive unexpected inputs our outputs the
    // will have a unity failure from CMock.
}
