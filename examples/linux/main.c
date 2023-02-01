#include "buffer.h"
#include "debug_msg.h"

int main() {
    rgb_buffer_t *buf = init_rgb_buffer(STATIC_BUFFER_LENGTH);
    if (!buf) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    write_rgb_buffer(buf);
    read_rgb_buffer(buf);

    return 0;
}