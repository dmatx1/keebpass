#ifndef KEEPBASS_H
#define KEEPBASS_H

#include "quantum.h"

#define KC_KPEX HYPR(KC_F23)
#define KC_KPCP HYPR(KC_F24)

#ifndef KEEPBASS_BUFFER_SIZE
#define KEEPBASS_BUFFER_SIZE 128
#endif

#ifndef KP_FIRST
#define KP_FIRST SAFE_RANGE
#endif

enum
{
  KP_PAST = KP_FIRST, // paste
  KP_PACL,            // paste and clear buffer
};

bool keepbass_process_record(uint16_t keycode, keyrecord_t* record);
void keepbass_process_hid(uint8_t* data, uint8_t length);

#endif
