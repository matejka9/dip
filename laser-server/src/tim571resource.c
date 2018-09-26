#include "tim571resource.h"
#include "tim571protocol.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define ONE_REQUEST_FILE "./example_tim571_sentence.txt"
#define FILE_TEST "./test.txt"
#define FILE_PATH ONE_REQUEST_FILE

FILE *fp;

int open_file_for_read()
{
    fp = fopen(FILE_PATH, "r");
    if (fp == NULL) {
    printf("No file with path: %s\n", FILE_PATH);
    return -1;
    }
    return 0;
}

int init_tim571resource()
{
    return open_file_for_read();
}

void deinit_tim571resource()
{
    if (fp != NULL)
        fclose(fp);
}

int get_tim571resource(char *buffer, int buffer_size)
{
    char c;
    int nread = 0, start = 0;

    while (nread < buffer_size) {
        c = fgetc(fp);

        if (c == EOF) {
            printf("Rewind file.\n");
            nread = 0;
            rewind(fp);
        }

        if (c == START_CHAR)
            start = 1;
        else if (start == 0)
            continue;

        buffer[nread++] = c;

        if (c == END_CHAR)
            break;
    }
    return nread;
}
