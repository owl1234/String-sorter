#include <sys/stat.h>

#ifndef COMMON_H
#define COMMON_H

#include "operation_codes.h"

const char name_input_file_ass[]  = "input_ass.txt";
const char name_output_file_ass[] = "output_ass.txt";

const char name_input_file_disass[]  = "input_disass.txt";
const char name_output_file_disass[] = "output_disass.txt";

const int MAX_SIZE = 10;
const int MAX_SIZE_RAM = 100000;
const int ERROR_NUMBER = -3802;
const int OK_FILE = 0;
const int OK = 0;
const int number_of_register_vars = 4;
const int number_of_commands = 9;
const int MAX_COUNT_LABELS = 20;

const char SEPARATORS[] = "\n ";

enum FLAGS_OF_THE_END_LINE {
    END_LINE = 0,
    SPACE    = 1,
    NOTHING  = 2
};

enum TYPE_OF_ARGUMENT {
    NOT_ARGS    = 0, // for pop
    IS_ELEM_T   = 1,
    IS_REGISTER = 2,
    IS_RAM      = 4,
};

struct File {
    FILE* ptr_to_file;
    const char* name;
    char* text;
    char* copy_of_text;
    int lines;
    struct stat information;
};

struct Label {
    char* name;
    int address; //symbol_address;
    int byte_address;
    int type_of_command;
};

#endif // COMMON_H

int is_right_command(const char* line, const char* command);

int number_of_symbols(char* buffer, char separator);

int file_construct(File* file, const char* name_file);

int type_of_value(const char* operation);

int get_number_of_register(const char* text);
