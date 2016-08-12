

char* get_key();

unsigned long getLibaddr() __attribute__((visibility("hidden")));

void init_getString() __attribute__((constructor)) __attribute__((visibility("hidden")));

void test_encrypt_section() __attribute__((section(".encryptsection"))) __attribute__((visibility("hidden")));


void g_message(const char * pszFormat, ...);