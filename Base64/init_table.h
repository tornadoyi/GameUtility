#include <string.h>

static const char *default_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static void inline init_table(int seed, base64 *_base64) {
    memcpy(_base64->table, default_table, 0x40);
    if (seed == 0) {
        return;
    }
    _base64->srand(seed);
    for (int i = 0;i < 0x40;i++) {
        int j = _base64->rand() % 0x40;
        int temp = _base64->table[i];
        _base64->table[i] = _base64->table[j];
        _base64->table[j] = temp;
    }
}

