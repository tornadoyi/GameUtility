#include "const.h"

int inline _strlen(const char *str) {
    int i;
    for (i = 0;str[i] != '\0';i++);
    return i;
}

int inline _strcmp(const char *temp1, const unsigned char *temp2) {
    int i;
    const unsigned char *str1 = reinterpret_cast<const unsigned char *>(temp1);
    const unsigned char *str2 = reinterpret_cast<const unsigned char *>(temp2);
    for (i = 0;str1[i] != '\0' && (str2[i] ^ 0xff) != '\0';i++) {
        if (str1[i] != (str2[i] ^ 0xff)) {
            return -1;
        }
    }
    if (str1[i] == str2[i]) {
        return 0;
    }
    return -1;
}

int inline _memcpy(char *dst, const unsigned char *src, int size) {
    int i;
    for (i = 0;i < size;i++) {
        dst[i] = static_cast<const char>(src[i] ^ 0xff);
    }
    return i;
}

int getConst(const char *name, char *buf, int buf_size) {
    int len;
    const unsigned char *value;
    static const unsigned char const_key[] = { 138, 146, 158, 202, 198, 138, 153, 139, 166, 168, 184, 145, 148, 168, 150, 175, 190, 144, 201, 135, 174, 181, 152, 141, 144, 201, 149, 207, 141, 181, 152, 177, 167, 199, 147, 183, 168, 205, 158, 185, 205, 138, 148, 194 };
    static const unsigned char const_key_name[] = { 180, 186, 166, 0 };
    static const unsigned char const_iv[] = { 173, 150, 134, 180, 156, 184, 208, 186, 139, 156, 141, 187, 154, 200, 180, 155, 151, 165, 148, 189, 136, 169, 194, 194 };
    static const unsigned char const_iv_name[] = { 182, 169, 0 };
    static const unsigned char const_key_salt[] = { 137, 154, 200, 137, 136, 180, 133, 198, 137, 201, 180, 194 };
    static const unsigned char const_key_salt_name[] = { 180, 186, 166, 160, 172, 190, 179, 171, 0 };
    static const unsigned char const_default_secret[] = { 144, 178, 178, 181, 180, 202, 208, 141, 198, 140, 179, 205, 153, 205, 171, 172, 135, 212, 136, 145, 177, 201, 142, 181, 136, 152, 153, 146, 138, 166, 133, 134, 147, 175, 185, 153, 137, 205, 173, 182, 199, 174, 189, 194 };
    static const unsigned char const_default_secret_name[] = { 187, 186, 185, 190, 170, 179, 171, 160, 172, 186, 188, 173, 186, 171, 0 };
    static const unsigned char const_prefs_key[] = { 200, 189, 146, 139, 200, 153, 134, 186, 149, 152, 189, 142, 156, 207, 152, 135, 139, 150, 177, 169, 142, 201, 145, 171, 133, 151, 204, 175, 171, 153, 185, 206, 149, 133, 212, 212, 148, 171, 167, 175, 158, 141, 182, 194 };
    static const unsigned char const_prefs_key_name[] = { 175, 173, 186, 185, 172, 160, 180, 186, 166, 0 };
    static const unsigned char const_prefs_iv[] = { 141, 143, 152, 171, 203, 187, 137, 157, 185, 169, 183, 190, 168, 201, 153, 167, 170, 179, 155, 212, 140, 189, 194, 194 };
    static const unsigned char const_prefs_iv_name[] = { 175, 173, 186, 185, 172, 160, 182, 169, 0 };

    if (_strcmp(name, const_key_name) == 0) {
        value = const_key;
        len = sizeof(const_key);
    }
    else if (_strcmp(name, const_iv_name) == 0) {
        value = const_iv;
        len = sizeof(const_iv);
    }
    else if (_strcmp(name, const_key_salt_name) == 0) {
        value = const_key_salt;
        len = sizeof(const_key_salt);
    }
    else if (_strcmp(name, const_default_secret_name) == 0) {
        value = const_default_secret;
        len = sizeof(const_default_secret);
    }
    else if (_strcmp(name, const_prefs_key_name) == 0) {
        value = const_prefs_key;
        len = sizeof(const_prefs_key);
    }
    else if (_strcmp(name, const_prefs_iv_name) == 0) {
        value = const_prefs_iv;
        len = sizeof(const_prefs_iv);
    }
    else {
        return -1;
    }

    if (len + 1 > buf_size) {
        return -1;
    }

    _memcpy(buf, value, len);
    buf[len] = '\0';
    return len;
}

