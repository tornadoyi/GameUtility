#include "base64.h"
#include "init_table.h"

extern "C" {
    int base64Encode(const char *input, int input_size, char *buf, int buf_size);
    int base64Decode(const char *input, int input_size, char *buf, int buf_size);
}

class initializer {
public:
    inline initializer() {
        _base64 = new base64();
        init_table(9144580, _base64);
    }
    inline ~initializer() {
        delete _base64;
    }
    base64 *_base64;
};

