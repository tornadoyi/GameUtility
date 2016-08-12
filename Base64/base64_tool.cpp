#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"
#include "init_table.h"

void usage() {
    printf("usage: ./base64_tool encode 100 hoge\n");
    printf("usage: ./base64_tool decode 100 bG6dm2== \n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return -1;
    }


    base64 _base64;
    int seed = 0;
    if (argc > 2) {
        seed = atoi(argv[2]);
    }
    init_table(seed, &_base64);

    int size, output_size;

    char *str;
    if (argc > 3) {
        str = argv[3];
        size = strlen(str);
    }
    else {
        str = new char[2048];
        for (size = 0;;size++) {
            int r = fgetc(stdin);
            if (r == EOF) {
                if (str[size-1] == 0x0a) {
                    size--;
                }
                break;
            }
            str[size] = r;
        }
    }

    char *method = argv[1];
    char buf[2048];
    if (strcmp(method, "encode") == 0) {
        output_size = _base64.encode(str, size, buf, 2048);
    } else if (strcmp(method, "decode") == 0) {
        output_size = _base64.decode(str, size, buf, 2048);
    } else if (strcmp(method, "encode2") == 0) {
        char decoded_value[2048];
        int decoded_value_size;
        init_table(0, &_base64);
        decoded_value_size = _base64.decode(str, size, decoded_value, 2048);
        init_table(seed, &_base64);
        output_size = _base64.encode(decoded_value, decoded_value_size, buf, 2048);

        // 正しくできているかのチェック
        char buf2[2048];
        int temp = _base64.decode(buf, output_size, buf2, 2048);
        if (temp != decoded_value_size || memcmp(decoded_value, buf2, temp) != 0) {
            fprintf(stderr, "入力を正常にエンコードできませんでした\n");
            return -1;
        }
    } else {
        usage();
        return -2;
    }

    for (int i = 0;i < output_size;i++) {
        fputc(buf[i], stdout);
    }
    fputc(0x0a, stdout);

    return 0;
}

