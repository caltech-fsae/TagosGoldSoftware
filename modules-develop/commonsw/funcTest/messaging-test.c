// USAGE:
// gcc -o messaging-test messaging-test.c car-messaging2.c
// ./messaging-test

#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include "messaging.h"

// example callback function 1
void callback_i1(char key, int val) {
	printf("callback i1\n");
	printf("key %i: value %i\n", key, val);
}

// example callback function 2
void callback_i2(char key, int val) {
	printf("callback i2\n");
	printf("key %i: value %i\n", key, val);
}

// example callback function 3
void callback_f1(char key, float val) {
	printf("callback f1\n");
	printf("key %i: value %f\n", key, val);
}

void callback_i_all(char key, int val) {
	printf("callback i all\n");
	printf("key %i: value %i\n", key, val);
}
void callback_f_all(char key, float val) {
	printf("callback f all\n");
	printf("key %i: value %f\n", key, val);
}

int main() {

	// associate callbacks with message_types
	register_callback_float(0, callback_f1);
	register_callback_int(1, callback_i1);
	register_callback_int(1, callback_i2);

	register_callback_int_all(callback_i_all);
	register_callback_float_all(callback_f_all);

	// send float with one attached callback
	send_float(5,1,0,12,3.1);

	// send int with two attached callbacks
	send_int(6,23,1,3,4061);

	// error testing
	send_int(20,23,1,5,1); // dest too high
	send_int(2,220,23,56,1); // priority too high
	register_callback_int(2, callback_i2);
	register_callback_int(2, callback_i2);
	register_callback_int(2, callback_i2);
	register_callback_int(2, callback_i2);
	register_callback_int(2, callback_i2);
	register_callback_int(2, callback_i2); // too many callbacks


    return 0;
}
