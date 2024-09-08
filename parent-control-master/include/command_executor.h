#ifndef COMMAND_EXECUTER_H
#define COMMAND_EXECUTER_H

enum command_executor_type
{
    HELP,
    KILL,
    QUIT,
    OUT_LOG,
    OUT_ERRLOG,
    RUNTIME_FILE,
    SWITCH_STATE,
};

void command_executor(enum command_executor_type type, const char *path);

#endif // COMMAND_EXECUTOR_H