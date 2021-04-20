#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

char* get_year_dir()
{
    char* year = malloc(sizeof(char) * 32);
    time_t now = time(NULL);
    struct tm *tm_struct = localtime(&now);
    int year_nb = tm_struct->tm_year + 1900;
    sprintf(year, "data/inventory/year%d", year_nb);
    DIR* d;
    if ((d = opendir(year)) == NULL) {
        mkdir(year, 0777);
    } else {
        closedir(d);
    }
    return year;
}

char* get_file_name(char* dir_name)
{
    time_t now = time(NULL);
    struct tm *tm_struct = localtime(&now);
    int month = tm_struct->tm_mon;
    char* file_name;


    file_name = malloc(sizeof(char) * 64);
    if (month < 10) {
        if (month == 0) sprintf(file_name, "%s/12.xml",dir_name);
        else sprintf(file_name, "%s/0%d.xml",dir_name, month);
    } else {
        sprintf(file_name, "%s/%d.xml",dir_name, month);
    }
    free(dir_name);

    return file_name;
}

int main()
{
    DIR *folder;
    struct dirent *entry;
    char* file_name;

    folder = opendir("data/inventory");
    if(folder == NULL)
    {
        perror("Unable to read directory");
        return(1);
    }

    file_name = get_file_name(get_year_dir());

    while( (entry=readdir(folder)) )
    {
        if (entry->d_type == 8) {
            long size2;
            char* new_content;
            char* filename = malloc(sizeof(char) * 32);
            strcpy(filename, "data/inventory/");
            strcat(filename, entry->d_name);

            FILE* f = fopen(filename, "r+");
            fseek(f, 0,SEEK_END);
            size2 = ftell(f);
            fseek (f, 0, SEEK_SET);
            new_content = malloc(sizeof(char) * size2);
            fread(new_content, 1, size2, f);
            fclose(f);

            //printf("%s", new_content);

            if(access(file_name, F_OK) == 0) {
                unsigned long ll = 0;
                unsigned long size;
                char* buffer;
                char* buffer_final;

                FILE* f3 = fopen(file_name, "r+");
                fseek(f3, 0,SEEK_END);
                size = ftell(f3);
                fseek (f3, 0, SEEK_SET);
                buffer = malloc(sizeof(char) * size);
                fread(buffer, 1, size, f3);
                fclose(f3);

                //printf("Size: %lu", size);

                for (unsigned long i = size-1; ll == 0 ; --i) {
                    if (buffer[i] == '\n') {
                        ll = i;
                    }
                }

                buffer[ll] = '\0';
                buffer_final = malloc(sizeof(char) * (size+strlen(new_content)));
                strcpy(buffer_final, buffer);

                char* tmp = strchr(new_content, '\n');
                char* tmp2 = strchr(tmp+1, '\n');

                strcat(buffer_final, "\n");
                strcat(buffer_final, tmp2+1);

                f = fopen(file_name, "w");
                fputs(buffer_final, f);
                fclose(f);

                //printf("%s", buffer);

                free(buffer);
                free(buffer_final);

            } else {

                FILE* f2 = fopen(file_name, "w");
                fputs(new_content, f2);
                fclose(f2);

            }
            free(new_content);
            remove(filename);
        }
    }

    closedir(folder);
    free(file_name);

    return 0;
}