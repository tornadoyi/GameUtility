#include "base64_plugin.h"

static initializer _initializer;
int base64Encode(const char *input, int input_size, char *buf, int buf_size) {
    return _initializer._base64->encode(input, input_size, buf, buf_size);
}
int base64Decode(const char *input, int input_size, char *buf, int buf_size) {
    return _initializer._base64->decode(input, input_size, buf, buf_size);
}

