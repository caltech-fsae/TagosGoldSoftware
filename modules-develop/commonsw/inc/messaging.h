/*
 * Internal messaging library
 * Caltech Racing 2016
 */
#ifndef CAR_MESSAGING_H
#define CAR_MESSAGING_H

#include <common.h>

#define N_MAX_FUNCTION_PER_CALLBACK 5   // a single message type can be attached to N many int or float callbacks

#if ( CAN_HEADER_LENGTH == 11)
#define N_DESTINATIONS      7
#define N_DESTINATION_BITS  3           // number of bits needed for desired number of destinations
#define N_PRIORITIES        32
#define N_PRIORITY_BITS     5
#define HEAD_SENDER_MASK    0x0038
#define HEAD_PRIORITY_MASK  0x07C0
#else /* assume CAN_HEADER_LENGTH == 20 */
#define N_DESTINATIONS      15
#define N_DESTINATION_BITS  4           // number of bits needed for desired number of destinations
#define N_PRIORITIES        1024
#define N_PRIORITY_BITS     12
#define HEAD_SENDER_MASK    0x00F0
#define HEAD_PRIORITY_MASK  0x3F00
#endif

#define PRIORITY_LOWEST          ( N_PRIORITIES - 1 )
#define HEAD_DESTINATION_SHIFT   0
#define HEAD_SENDER_SHIFT        ( HEAD_DESTINATION_SHIFT + N_DESTINATION_BITS )
#define HEAD_PRIORITY_SHIFT      ( HEAD_SENDER_SHIFT + N_DESTINATION_BITS )

StatusInt register_callback_int( uint16_t message_type, void ( *callback )( uint8_t sender, uint16_t id, uint32_t val ) );
StatusInt register_callback_float( uint16_t message_type, void ( *callback )( uint8_t sender, uint16_t id, float val ) );
StatusInt register_callback_int_all( void ( *callback )( uint16_t type, uint8_t sender, uint16_t id, uint32_t val ) );
StatusInt register_callback_float_all( void ( *callback )( uint16_t type, uint8_t sender, uint16_t id, float val ) );
StatusInt send_int( uint8_t destination, uint16_t priority, uint16_t message_type, uint16_t id, uint32_t val );
StatusInt send_float( uint8_t destination, uint16_t priority, uint16_t message_type, uint16_t id, float val );
StatusInt init_messaging( void );

#endif // CAR_MESSAGING_H
