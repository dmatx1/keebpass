#include "keebpass.h"
#include "raw_hid.h"
#include <string.h>

static char keebpass_buffer[KEEBPASS_BUFFER_SIZE] = { 0 };
static uint8_t keebpass_buffer_length = 0;

void keebpass_process_hid(uint8_t* data, uint8_t length)
{
    if (length > 0 && data[0] == 0xfd)
    {
        length -= 2;
        int length1 = KEEBPASS_BUFFER_SIZE - keebpass_buffer_length - 1;
        if (length1 > length)
            length1 = length;
        memcpy(keebpass_buffer + keebpass_buffer_length, data + 1, length1);
        keebpass_buffer_length += length1;
    }
}

// support VIA
__attribute__((weak)) void raw_hid_receive(uint8_t *data, uint8_t length)
{
    keebpass_process_hid(data, length);
}

// support using custom HID handler
__attribute__((weak)) void raw_hid_receive_kb(uint8_t *data, uint8_t length)
{
    keebpass_process_hid(data, length);
}

bool keebpass_process_record(uint16_t keycode, keyrecord_t* record)
{
    if ((keycode == KL_PAST || keycode == KL_PACL) && record->event.pressed)
    {
        send_string_with_delay(keebpass_buffer, 10);
        if (keycode == KL_PACL)
        {
            keebpass_buffer_length = 0;
            memset(keebpass_buffer, 0, sizeof(keebpass_buffer));
        }

        return false;
    }

    return true;
}
