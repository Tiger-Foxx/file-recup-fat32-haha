#include "global.h"


int disk_info(void *disk_addr)
{
    BootEntry *DBR = (BootEntry *)disk_addr; 

    printf("Number of FATs = %d\n", DBR->BPB_NumFATs);
    printf("Number of bytes per sector = %d\n", DBR->BPB_BytsPerSec);
    printf("Number of sectors per cluster = %d\n", DBR->BPB_SecPerClus);
    printf("Number of reserved sectors = %d\n", DBR->BPB_RsvdSecCnt);

    return 0;
}


int root_dir(void *disk_addr, int flag, unsigned char **command)
{
    BootEntry *DBR = (BootEntry *)disk_addr; 

    
    int rsv_size = DBR->BPB_RsvdSecCnt * DBR->BPB_BytsPerSec;
    
    int fat_size = DBR->BPB_NumFATs * DBR->BPB_FATSz32 * DBR->BPB_BytsPerSec;

    unsigned int *FATs = disk_addr + rsv_size;                          
    unsigned char *data_addr = disk_addr + rsv_size + fat_size;         
    unsigned int cluster_id = DBR->BPB_RootClus;                        
    unsigned int clus_size = DBR->BPB_BytsPerSec * DBR->BPB_SecPerClus; 

    int file_count = 0;                 
    int file_found = 0;                 
    unsigned int end_clus = 0x0ffffff8; 
    
    unsigned char empty_file[SHA_DIGEST_LENGTH * 2] = "da39a3ee5e6b4b0d3255bfef95601890afd80709";

    unsigned char *target_file;
    char *target_ext;

    DirEntry *potential_entry;             
    unsigned int loc[5] = {0, 0, 0, 0, 0}; 
    if (flag == 2 || flag == 3 || flag == 4)
    {
        
        char *target = (char *)malloc(12);
        strcpy(target, command[0]);
        target_ext = NULL;
        target_file = strtok_r(target, ".", &target_ext);
    }

    
    while (cluster_id < end_clus) 
    {
        unsigned char *root_addr = data_addr + (cluster_id - 2) * DBR->BPB_SecPerClus * DBR->BPB_BytsPerSec; 

        int entry_index = 0;
        while (entry_index < DBR->BPB_BytsPerSec * DBR->BPB_SecPerClus && root_addr[entry_index] != 0)
        {
            DirEntry *cur_entry = (DirEntry *)(root_addr + entry_index);

            unsigned int file_size = cur_entry->DIR_FileSize;
            unsigned int start_clus = cur_entry->DIR_FstClusHI << 16 | cur_entry->DIR_FstClusLO;

            
            unsigned char *file_name = (unsigned char *)malloc(9);
            strncpy(file_name, cur_entry->DIR_Name, 8);
            file_name = strtok(file_name, " ");

            
            unsigned char *extension = (unsigned char *)malloc(4);
            strncpy(extension, cur_entry->DIR_Name + 8, 3);
            extension = strtok(extension, " ");

            if ((unsigned int)file_name[0] != 0xe5) 
            {
                
                if (flag == 1)
                {
                    if (cur_entry->DIR_Attr == 16) 
                        printf("%s/ (size = %d, starting cluster = %d)\n", file_name, file_size, start_clus);
                    else if (extension == NULL) 
                        printf("%s (size = %d, starting cluster = %d)\n", file_name, file_size, start_clus);
                    else 
                        printf("%s.%s (size = %d, starting cluster = %d)\n", file_name, extension, file_size, start_clus);
                    file_count++;
                }
            }
            else if (cur_entry->DIR_Attr != 16) 
            {
                
                if (strcmp(target_file + 1, file_name + 1) == 0 && ((extension == NULL && strcmp(target_ext, "") == 0) || (extension != NULL && strcmp((unsigned char *)target_ext, extension) == 0)))
                {
                    
                    if (flag == 2)
                    {
                        potential_entry = cur_entry;
                        file_found++;
                    }
                    
                    else if (flag == 3 || flag == 4)
                    {
                        
                        unsigned char *content = data_addr + (start_clus - 2) * clus_size;
                        unsigned char sha1_20[SHA_DIGEST_LENGTH];
                        SHA1(content, file_size, sha1_20);

                        
                        if (to_recover(sha1_20, command[2], file_size))
                        {
                            potential_entry = cur_entry;
                            file_found = 1;
                            flag = 3;
                            break;
                        }
                        
                        else if (flag == 4)
                        {
                            unsigned char *start_content = (unsigned char *)malloc(clus_size);
                            strncpy(start_content, content, clus_size);

                            file_found = recur_search(start_content, command[2], file_size, DBR, file_size - clus_size, data_addr, start_clus, FATs, clus_size);

                            if (file_found)
                                break;
                        }
                    }
                }
            }

            entry_index += 32; 
        }
        cluster_id = FATs[cluster_id];

        if ((flag == 3 || flag == 4) && file_found)
            break;
    }
    
    
    if (flag == 1)
        printf("Total number of entries = %d\n", file_count++);
    else
    {
        
        if (file_found == 1)
        {
            
            if (flag == 2 || flag == 3)
                recover_contiguous(potential_entry, FATs, target_file, clus_size);

            printf("%s: successfully recovered", command[0]);

            if (flag == 2)
                printf("\n");
            else if (flag == 3 || flag == 4)
                printf(" with SHA-1\n");
        }
        else if (file_found == 0)
            printf("%s: file not found\n", command[0]);
        else
            printf("%s: multiple candidates found\n", command[0]);
    }

    return 0;
}