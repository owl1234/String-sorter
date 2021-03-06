/**
 *  @file
 *  @author Kolesnikova Xenia <heiduk.k.k.s@yandex.ru>
 *  @version 1.0
 *  @par Last edition
 *                  November 1, 2020, 00:00:25
 *  @par What was changed?
 *                      1. HAPPY HALLOWEEN !!!
 *                      2. Add ram.
 *                              push [rax] - take the contents of the register, contact the address in ram and make push
 *                              push [4]   - contact address 4 in ram and make push
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "assembler.h"
#include "disassembler.h"
#include "processor.h"
#include "common.h"

void help() {
    printf("This program sorts strings from file in alphabetical order (without taking into account the signs of pre).\n"
           "The required parameters look like this:\n"
                    "[-h] - if you want to read this help again\n"
                    "[-a] - if you want to assembler file\n"
                    "[-d] - if you want to disassembler file\n"
                    "[-p] - if you want to evaluate expression\n"
                    "For more information, go here: https://github.com/owl1234/MIPT_projects_1_sem/tree/master/CPU\n");
}

int main(const int argc, const char* argv[]) {
    assert(argc == 2);

    if(!strcmp(argv[1], "-a")) {
        assembling_file();
    } else if(!strcmp(argv[1], "-d")) {
        disassembling_file();
    } else if(!strcmp(argv[1], "-p")) {
        File input_file = {};

        int status = file_construct(&input_file, name_output_file_ass);
        if(status != OK_FILE) {
            return ERROR_NUMBER;
        }

        file_handler(input_file);
    } else {
        help();
    }

    return 0;
}

