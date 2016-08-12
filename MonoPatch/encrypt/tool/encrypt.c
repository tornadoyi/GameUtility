/*
 * encrypt_section.c
 *
 *  Created on: 2014-8-29
 *      Author: lyz
 */
 
#include <stdio.h>
#include <fcntl.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "tea.h"

static int loglevel = 1;

void LOG(int level, const char * pszFormat, ...)
{
    if(level < loglevel) return;
    char buf[2048];

    va_list args;
    va_start(args, pszFormat);
    vsnprintf(buf, 2048, pszFormat, args);
    va_end(args);

    if(level == 2)
        printf("error: %s\n", (const char*)&buf);
    else
        printf("%s\n", (const char*)&buf);
}

#define DEBUG(fmt, args...) LOG(0, fmt, ##args)
#define INFO(fmt, args...) LOG(1, fmt, ##args)
#define ERROR(fmt, args...) LOG(2, fmt, ##args)

void usage()
{
    INFO("Uage: ");
    INFO("encrypt [so filename] [section name] [key]");
}

int main(int argc, char** argv){
 
    char *shstr = NULL;
    char *content = NULL;
 
    Elf32_Ehdr ehdr;
    Elf32_Shdr shdr;
 
    int i;
    unsigned int base = 0, length = 0;
    unsigned short nblock;
    unsigned short nsize;
    unsigned char block_size = 16;
 
    int fd;
 
    if(argc < 4){
        usage();
        return -1;
    }

    char* filename = argv[1];
    char* section_name = argv[2];
    char* key = argv[3];

    DEBUG("filename: %s", filename);
    DEBUG("section name: %s", section_name);
    DEBUG("key: %s", key);

    if(argc > 4)
    {
        char* debug = argv[4];
        if(strcmp(debug, "debug") == 0)
        {
            loglevel = 0;
        }
    }

 
    fd = open(filename, O_RDWR);
 
    if(fd < 0){
        ERROR("open %s failed!", argv[1]);
        goto _error;
    }
    DEBUG("open %s success!", argv[1]);
 
    if(read(fd, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)){
        ERROR("Read Elf header failed!");
        goto _error;
    }
    DEBUG("Read Elf header success!");
 
    lseek(fd, ehdr.e_shoff + sizeof(Elf32_Shdr) * ehdr.e_shstrndx, SEEK_SET);  //定位section header
 
    if(read(fd, &shdr, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)){
        ERROR("Read Section string table failed!");
        goto _error;
    }
    DEBUG("Read Section string table success!");
 
    if((shstr = (char *)malloc(shdr.sh_size)) == NULL){
        ERROR("malloc space(%d bytes) for section string table failed!", shdr.sh_size);
        goto _error;
    }
 
    lseek(fd, shdr.sh_offset, SEEK_SET);  //定位string table的位置
    if(read(fd, shstr, shdr.sh_size) != shdr.sh_size){
        ERROR("Read string table failed!");
        goto _error;
    }
    DEBUG("Read string table success!");
 
    lseek(fd, ehdr.e_shoff, SEEK_SET);
    for(i = 0; i < ehdr.e_shnum; i++){        
        if(read(fd, &shdr, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)){
            ERROR("Find section .textprocedure failed!");
            goto _error;
        }
        DEBUG("%d: %s", i+1, shstr + shdr.sh_name);
        if(strcmp(shstr + shdr.sh_name, section_name) == 0) {
            if(base != 0)
            {
                ERROR("Find repeated section %s", section_name);
                return -1;
            }
            base = shdr.sh_offset;
            length = shdr.sh_size;
        }
    }
    
    if(length == 0)
    {
        ERROR("Can not find section %s", section_name);
        return -1;
    }

    lseek(fd, base, SEEK_SET);
    content = (char*)malloc(length);
    if(content == NULL){
        ERROR("malloc space for content failed!");
        goto _error;
    }
 
    if(read(fd, content, length) != length){
        ERROR("Read section %s failed!", section_name);
        goto _error;
    }
    DEBUG("Read section %s success!", section_name);
 
    nblock = length / block_size;
    nsize = base / 4096 + (base % 4096 == 0 ? 0 : 1);
 
    DEBUG("base = %d, length = %d", base, length);
    DEBUG("nblock = %d, nsize = %d", nblock, nsize);

    ehdr.e_entry = (length << 16) + nsize;
    ehdr.e_shoff = base;
    
    // encrypt
    tea_encrypt(key, (unsigned char*)content, length);
    
    char* dcontent = (char*)malloc(length*2+1);
    memset(dcontent, 0, sizeof(char)*(length*2+1));
    for(int i=0; i<length; ++i)
    {
        unsigned char h = (unsigned char)content[i];
        char byte2[3] = {0};
        //printf("%d = %02x\n", h, h);
        sprintf(byte2, "%02x", h);
        dcontent[i*2] = byte2[0];
        dcontent[i*2+1] = byte2[1];
    }
    printf("\n");
    printf("%s", dcontent);
    printf("\n");

 
    lseek(fd, 0, SEEK_SET);
    if(write(fd, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)){
        ERROR("Write Elf header to so failed!");
        goto _error;
    }
 
    lseek(fd, base, SEEK_SET);
    if(write(fd, content, length) != length){
        ERROR("Write modified content to so failed!");
        goto _error;
    }
    DEBUG("Write modified content to so success!");
 
    INFO("Encrypt %s Completed!", filename);
 
_error:
    close(fd);
    free(content);
    free(shstr);
    return 0;
 
}