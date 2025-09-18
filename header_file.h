#ifndef HEADER_FILE
#define HEADER_FILE
#include<stdio.h>
#include<string.h>


int read_tags(FILE* fptr);
int  copy_remain(FILE* src,FILE* des, char* usr_str,char* usr_tag);
int check_id3_version(FILE* fptr);

#endif