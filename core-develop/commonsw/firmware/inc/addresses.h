/*
 * Module Addressing Definitions
 * Caltech Racing 2016
 */
#ifndef ADDRESSES_H
#define ADDRESSES_H

/* Length of the CAN header (11 or 20 bits) */
#define CAN_HEADER_LENGTH   11

/* Section of the header that contains the address */
#if (CAN_HEADER_LENGTH == 11)
#define CAN_ADDRESS_MASK    0x0007
#else /* assume CAN_HEADER_LENGTH == 20 */
#define CAN_ADDRESS_MASK    0x000F
#endif
#define CAN_ADDRESS_SHIFT   (32 - CAN_HEADER_LENGTH)



/* Internal system addesses */
/* !!! System is limited to 7 devices with 11 bits of header, or 15 with 20 bits !!! */
#define ADDRESS_BROADCAST   0
#define ADDRESS_CORE        1
#define ADDRESS_COMM        2
#define ADDRESS_SHUTDOWN    3
#define ADDRESS_IO_PEDALS_1 4
#define ADDRESS_IO_PEDALS_2 5
#define ADDRESS_IO_ACCUM    6

             

#define N_CAN_DEVICES       6 /* Must be <= 7 (11 bits) or 15 (20 bits)! */

/* External addresses */
#define ADDRESS_BMS         (0x20)
#define ADDRESS_MOTOR       (0x195)
#define CAN_ADDRESS_BMS_MASK    ((uint32_t) 0x7F8)

#endif /* ADDRESSES_H */
