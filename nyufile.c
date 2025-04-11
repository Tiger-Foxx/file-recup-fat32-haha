
#include "global.h"

int main(int argc, char *argv[])
{
    unsigned char **command = (unsigned char **)malloc(sizeof(char *) * 4);
    int usage = 0;
    int options[] = {0, 0, 0, 0, 0};
    int ch;

    opterr = 0; 
    
    while ((ch = getopt(argc, argv, "ilr:R:s:")) != -1)
    {
        switch (ch)
        {
        case 'i':
            options[0]++;
            break;
        case 'l':
            options[1]++;
            break;
        case 'r':
            options[2]++;
            command[0] = optarg;
            break;
        case 'R':
            options[3]++;
            command[0] = optarg;
            break;
        case 's':
            options[4]++;
            command[2] = optarg;
            break;
        }
    }

    
    if (optopt)
        usage = 1;
    
    else if (argc - optind != 1)
        usage = 1;
    else
    {
        int fd = open(argv[optind], O_RDWR, 0644); 

        if (fd < 0)
        {
            printf("File not exist");
            return 0;
        }

        
        struct stat buf_size;
        fstat(fd, &buf_size);

        void *addr = mmap(NULL, buf_size.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);

        
        if (options[2] == 0 && options[3] == 0 && options[4] == 0 && (options[0] + options[1] == 1))
        {
            
            if (options[0])
                disk_info(addr);
            
            else
                root_dir(addr, 1, NULL);
        }
        
        else if (options[2] == 1 && options[0] == 0 && options[1] == 0 && options[3] == 0)
        {
            
            if (options[4] == 0)
                root_dir(addr, 2, command);
            
            else if (options[4] == 1)
                root_dir(addr, 3, command);
        }
        
        else if (options[3] == 1 && options[4] == 1 && options[0] == 0 && options[1] == 0 && options[2] == 0)
            root_dir(addr, 4, command);
        else
            usage = 1;

        munmap(addr, buf_size.st_size);
    }

    
    if (usage)
    {
        printf("Usage: ./nyufile disk <options>\n"
               "  -i                     Print the file system information.\n"
               "  -l                     List the root directory.\n"
               "  -r filename [-s sha1]  Recover a contiguous file.\n"
               "  -R filename -s sha1    Recover a possibly non-contiguous file.\n");
    }

    free(command);
    return 0;
}