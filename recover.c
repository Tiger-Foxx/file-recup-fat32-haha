#include "global.h"

int recur_search(unsigned char *content, unsigned char *input_sha1, unsigned int file_size, BootEntry *DBR, int remain_size, unsigned char *data_addr, unsigned int clus_index, unsigned int *FATs, unsigned int loc)
{
    if (remain_size <= 0)
        return 0;

    unsigned int clus_size = DBR->BPB_BytsPerSec * DBR->BPB_SecPerClus; 
    unsigned int end_clus = 0x0ffffff8;                                 

    printf("Recur-search %d\n", clus_index);
    for (unsigned int a = clus_index + 1; a < 22; a++)
    {
        if (FATs[a] == 0) 
        {
            printf("A deleted Cluster %d\n", a);
            unsigned char *merge_content = (unsigned char *)malloc(5 * clus_size);
            strcpy(merge_content, content);

            unsigned char *cur_content = data_addr + (a - 2) * clus_size;
            
            if (remain_size <= clus_size)
            {
                printf("Last Cluster\n");
                strncpy(merge_content + loc, cur_content, remain_size);
                
                unsigned char sha1_20[SHA_DIGEST_LENGTH];
                SHA1(merge_content, file_size, sha1_20);

                
                if (to_recover(sha1_20, input_sha1, file_size))
                {
                    FATs[clus_index] = a;
                    FATs[a] = end_clus;
                    return 1;
                }
            }
            else
            {
                strncpy(merge_content + loc, cur_content, clus_size);

                if (recur_search(merge_content, input_sha1, file_size, DBR, remain_size - clus_size, data_addr, a, FATs, loc + clus_size))
                {
                    FATs[clus_index] = a;
                    return 1;
                }
            }

            free(merge_content);
        }
    }
    return 0;
}

int to_recover(unsigned char *sha1_20, unsigned char *input_sha1, int file_size)
{
    
    unsigned char empty_file[SHA_DIGEST_LENGTH * 2] = "da39a3ee5e6b4b0d3255bfef95601890afd80709";
    unsigned char sha1_40[SHA_DIGEST_LENGTH * 2];

    
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf(sha1_40 + i * 2, "%02x", sha1_20[i]);

    printf("%s\n", sha1_40);

    return (strcmp(input_sha1, sha1_40) == 0 || (file_size == 0 && strcmp(input_sha1, empty_file) == 0));
}

void recover_contiguous(DirEntry *cur_entry, unsigned int *FATs, char *target_file, unsigned int clus_size)
{
    unsigned int file_size = cur_entry->DIR_FileSize;
    unsigned int start_clus = cur_entry->DIR_FstClusHI << 16 | cur_entry->DIR_FstClusLO;
    unsigned int end_clus = 0x0ffffff8; 

    cur_entry->DIR_Name[0] = target_file[0]; 

    
    if (file_size <= clus_size)
        FATs[start_clus] = end_clus;
    
    else
    {
        for (int i = 0; i < file_size - clus_size; i += clus_size)
        {
            FATs[start_clus] = start_clus + 1;
            start_clus++;
        }
        FATs[start_clus] = end_clus;
    }
}

void recover_non_contiguous(DirEntry *cur_entry, unsigned int *FATs, char *target_file, unsigned int clus_size, unsigned int *loc)
{
    unsigned int file_size = cur_entry->DIR_FileSize;
    unsigned int start_clus = cur_entry->DIR_FstClusHI << 16 | cur_entry->DIR_FstClusLO;
    unsigned int end_clus = 0x0ffffff8; 

    cur_entry->DIR_Name[0] = target_file[0]; 

    int i = 0;
    while (loc[i] != 0)
    {
        FATs[start_clus] = loc[i];
        start_clus = loc[i];
    }
    FATs[start_clus] = end_clus;
}