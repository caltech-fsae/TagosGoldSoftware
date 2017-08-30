#ifndef MESSAGE_CALLBACKS_NOFILTER_H   /* Include guard */
#define MESSAGE_CALLBACKS_NOFILTER_H

#include <stdint.h>
#include <common.h>

StatusInt init_messages_nofilter(void);
void float_message_nofilter_cb(uint16_t type, uint8_t sender, uint16_t id, float payload);
void int_message_nofilter_cb(uint16_t type, uint8_t sender, uint16_t id, uint32_t payload);

#endif // MESSAGE_CALLBACKS_NOFILTER_H
