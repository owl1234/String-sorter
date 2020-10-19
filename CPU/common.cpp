#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "common.h"

int is_right_command(const char* line, const char* command) {
    int length_command = strlen(command);

    for(int i=0; i<length_command; ++i) {
        if(command[i] != line[i]) {
            return -1;
        }
    }

    return 0;
}

int number_of_symbols(char* buffer, char separator) {
    char now_symbol = *buffer;
    int number = 0;

    while(now_symbol != '\0') {
        now_symbol = *buffer;
        if(now_symbol == separator) {
            ++number;
        }

        ++buffer;
    }

    return number;
}

int file_construct(File* file, const char* name_file) {
    assert(file);
    assert(name_file);

    file->name = name_file;
    stat(file->name, &(file->information));

    file->text = (char*)calloc(file->information.st_size + 2, sizeof(char));

    file->ptr_to_file = fopen(file->name, "r");
    int status = fread(file->text, sizeof(char), file->information.st_size, file->ptr_to_file);
    if(status != file->information.st_size) {
        printf("File don't read normal!\n");
        return ERROR_NUMBER;
    }

    fclose(file->ptr_to_file);

    file->lines = number_of_symbols(file->text, '\n');
    return OK_FILE;
}