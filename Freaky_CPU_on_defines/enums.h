#include "processor.h"

#define DEFINE_COMMANDS(name, number, arg, code_assembler, code_disassembler) \
        COMMAND_##name = number,

typedef enum my_commands_for_processor {
    #include "COMMANDS.H"
} commands_for_processor;

#undef DEFINE_COMMANDS
