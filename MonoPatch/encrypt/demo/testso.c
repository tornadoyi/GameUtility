#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include <sys/mman.h>
#include <elf.h>
#include "testso.h"
#include "tea.h"

static char libname[] = "libtest.so";

char* get_key()
{
	test_encrypt_section();
	return "hello world";
}

unsigned long getLibaddr()
{
    g_message("here is get lib addr");

    unsigned long ret = 0;
    char buffer[4096];
    char *temp;
    int pid;
 
    FILE *fp;
    pid = getpid();
    sprintf(buffer, "/proc/%d/maps", pid);
    fp = fopen(buffer, "r");
    if(fp == NULL){
         g_message("Open %s failed!", buffer);
         goto _error;
    }

    while(fgets(buffer, sizeof(buffer), fp)){
        if(strstr(buffer, libname)){
            temp = strtok(buffer, "-");
            ret = strtoul(temp, NULL, 16);
            break;
        }
    }
_error:
    fclose(fp);
    return ret;
}
void init_getString()
{
	char key[] = "f02bda076db01587fab37efcd00bced9";
    char name[15];
    unsigned int nblock;
    unsigned int nsize;
    unsigned long lib_addr;
    unsigned long text_addr;
    unsigned int i;
    Elf32_Ehdr *ehdr;
    Elf32_Shdr *shdr;
    
    lib_addr = getLibaddr(); 
    g_message("libaddr is : %d", lib_addr);

 
    ehdr = (Elf32_Ehdr *)lib_addr;
    text_addr = ehdr->e_shoff + lib_addr;
    nblock = ehdr->e_entry >> 16;
    nsize = ehdr->e_entry & 0xffff;

    g_message("shum: %d", ehdr->e_shnum);
    
    g_message("text_addr is : %d", text_addr);
    g_message("nblock is : %d", nblock);
    g_message("nsize is : %d", nsize);
    
    if(mprotect((void*)lib_addr, 4096*nsize, PROT_EXEC | PROT_READ | PROT_WRITE) != 0){
        g_message("mem privilege change failed!");
    }

    g_message("mem privilege change success!");
 
    // decrypt
    tea_decrypt((const char*)&key, (unsigned char*)text_addr, nblock);
    
    
    if(mprotect((void*)lib_addr, 4096*nsize, PROT_EXEC | PROT_READ ) != 0){
        g_message("mem privilege change failed!");
    }
         
    g_message("mem privilege change success!");

}


void test_encrypt_section()
{
	g_message("encrypt content");
}




void g_message(const char * pszFormat, ...)
{
    char buf[2048];

    va_list args;
    va_start(args, pszFormat);
    vsnprintf(buf, 2048, pszFormat, args);
    va_end(args);

    FILE *fp = fopen("so.log", "w");
    fprintf(fp, "%s\n", (const char*)&buf); 
    fclose(fp);
    printf("%s\n", (const char*)&buf);
}


