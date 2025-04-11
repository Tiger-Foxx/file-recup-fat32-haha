#ifndef _RECOVER_H_
#define _RECOVER_H_

#include "global.h"

int recur_search(unsigned char *content, unsigned char *input_sha1, unsigned int file_size, BootEntry *DBR, int remain_size, unsigned char *data_addr, unsigned int clus_index, unsigned int *FATs, unsigned int loc);
int to_recover(unsigned char *sha1_20, unsigned char *input_sha1, int file_size);
void recover_contiguous(DirEntry *cur_entry, unsigned int *FATs, char *target_file, unsigned int clus_size);
void recover_non_contiguous(DirEntry *cur_entry, unsigned int *FATs, char *target_file, unsigned int clus_size, unsigned int *loc);

#endif