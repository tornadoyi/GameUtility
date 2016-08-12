#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
  
  
/* 32-bit ELF base types. */
typedef unsigned int Elf32_Addr;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Off;
typedef signed int Elf32_Sword;
typedef unsigned int Elf32_Word;
  
  
  
  
#define EI_NIDENT 16
  
/*
 * ELF header.
 */
  
typedef struct {
  unsigned char  e_ident[EI_NIDENT];  /* File identification. */
  Elf32_Half  e_type;    /* File type. */
  Elf32_Half  e_machine;  /* Machine architecture. */
  Elf32_Word  e_version;  /* ELF format version. */
  Elf32_Addr  e_entry;  /* Entry point. */
  Elf32_Off  e_phoff;  /* Program header file offset. */
  Elf32_Off  e_shoff;  /* Section header file offset. */
  Elf32_Word  e_flags;  /* Architecture-specific flags. */
  Elf32_Half  e_ehsize;  /* Size of ELF header in bytes. */
  Elf32_Half  e_phentsize;  /* Size of program header entry. */
  Elf32_Half  e_phnum;  /* Number of program header entries. */
  Elf32_Half  e_shentsize;  /* Size of section header entry. */
  Elf32_Half  e_shnum;  /* Number of section header entries. */
  Elf32_Half  e_shstrndx;  /* Section name strings section. */
} Elf32_Ehdr;
  
/*
 * Section header.
 */
  
typedef struct {
  Elf32_Word  sh_name;  /* Section name (index into the section header string table). */
  Elf32_Word  sh_type;  /* Section type. */
  Elf32_Word  sh_flags; /* Section flags. */
  Elf32_Addr  sh_addr;  /* Address in memory image. */
  Elf32_Off sh_offset;  /* Offset in file. */
  Elf32_Word  sh_size;  /* Size in bytes. */
  Elf32_Word  sh_link;  /* Index of a related section. */
  Elf32_Word  sh_info;  /* Depends on section type. */
  Elf32_Word  sh_addralign; /* Alignment in bytes. */
  Elf32_Word  sh_entsize; /* Size of each entry in section. */
} Elf32_Shdr;


