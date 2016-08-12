#pragma once

#include <stdlib.h>
#include <string.h>

// このクラスでencode/decodeする前にtableを初期化する必要がある
class base64 {
public:
    inline base64();
    inline ~base64();

    int inline encode(const char *input, int input_size, char *buf, int buf_size);
    int inline decode(const char *input, int input_size, char *buf, int buf_size);

    char *table;

    // ランダム関数は実装によって違う値が出るため自分で実装する
    void inline srand(int seed);
    int inline rand();

private:
    int next;
    int inline get_index(char input);
};

inline base64::base64() {
    next = 0;
    table = new char[0x40];
}
inline base64::~base64() {
    delete [] table;
}

int inline base64::encode(const char *input, int input_size, char *buf, int buf_size) {
    int output_size = input_size * 4 / 3;
    if (output_size % 4 != 0) {
        output_size += 4 - output_size % 4;
    }

    if (output_size  > buf_size) {
        return -1;
    }

    char tmp;
    unsigned char current, next;

    int pos = 0;
    for (int i = 0;i < input_size;i += 3){
        tmp = input[i];
        current = *reinterpret_cast<unsigned char *>(&tmp);
        next = i+1 >= input_size ? 0 : input[i+1];
        buf[pos++] = table[current >> 2 & 0x3f];
        buf[pos++] = table[(current << 4 | next >> 4) & 0x3f];
        if (i + 1 >= input_size) {
            buf[pos++] = '=';
            buf[pos++] = '=';
            break;
        }

        current = next;
        next = i+2 >= input_size ? 0 : input[i+2];
        buf[pos++] = table[(current << 2 | next >> 6) & 0x3f];
        if (i + 2 >= input_size) {
            buf[pos++] = '=';
            break;
        } else {
            buf[pos++] = table[next & 0x3f];
        }
    }
    return pos;
}

int inline base64::decode(const char *input, int input_size, char *buf, int buf_size) {
    int output_size = input_size * 3 / 4;
    for (int index = input_size - 1;input[index] == '=';output_size--, index--);
    if (output_size > buf_size) {
        return -1;
    }

    int current, next;
    current = get_index(input[0]);
    if (current == -1) {
        return 0;
    }
    int pos = 0;
    for (int i = 0;i + 1 < input_size;i++) {
        next = get_index(input[i+1]);
        if (next == -1) {
            break;
        }
        switch (i % 4) {
            case 0:
                buf[pos++] = (current << 2 | next >> 4) & 0xff;
                break;
            case 1:
                buf[pos++] = (current << 4 | next >> 2) & 0xff;
                break;
            case 2:
                buf[pos++] = (current << 6 | next) & 0xff;
                break;
            case 3:
                break;
        }
        current = next;
    }
    return pos;
}

int inline base64::get_index(char input) {
    for (int i = 0;i < 0x40;i++) {
        if (table[i] == input) {
            return i;
        }
    }
    return -1;
}

void inline base64::srand(int seed) {
    this->next = seed;
}
int inline base64::rand() {
    next = next * 1103515245 + 12345;
    return ((unsigned)(next / 65536)) % 32768;
}

