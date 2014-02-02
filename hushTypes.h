#include <string.h>

typedef jobsVal unsigned short;
typedef cmdVal unsigned short;
typedef errVal int; //because my error types are declared in an enum
typedef bool_t unsigned short;

struct limitSpec {
    const jobsVal jobs = 64;
    const cmdVal cmdLen = 1000;
    const unsigned int numOptions = 100;
    const unsigned int optionLen = 200;
    const unsigned int numArgs = 100;
    const unsigned int argLen = 200;
} hushLimits;

struct sysEnv {
    char* ARCH;
    char* PATH;
    char* SHELL;
    char* PWD;
    char* OLDPWD;
    char* PAGER;
    char* EDITOR;
    char* HOME;
    char* USER;
    char* PS1;
} hushEnv;

typedef struct jobItem {
    pid_t pid;
    char* tty;
    char* cmdStr; 
    command_t cmd;
}

typedef struct command_t {
    char* name;
    char** optionsNargs;
    bool_t isBackground;
}

struct State {
    bool_t isInteractivate;
    bool_t isRunning;
    enum { okComputer, unknownArch, undefinedOption, commandNotFound } hushErrno;
    jobItem jobs[jobCap];
    jobsVal jobCount;
    char** dirStack;
} hushState;

char** hushBuiltins = {"cd", "pwd", "pushd", "popd", "for"}
unsigned int numBuiltins = strlen(hushBuiltins);
//Matters of Strategy:
//   I will not typedef structs which will have only one instantiation.
