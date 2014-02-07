#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>

//What is the meaning of all the struct words?
#define CMD_STR_LIM 300
#define CMD_REP_LIM 20
#define CMD_WORD_LIM 20
#define CMD_WORD_LEN_LIM 40
#define JOB_MAX 32
#define HIST_SIZE 1000
#define DIR_STACK_MAX 16
#define DYN_VAR_MAX 100
#define NUM_BUILTINS 10
#define NUM_ENV_DEPS 1
#define BUILTIN_WORD_LEN 16
#define TIMESTAMP_LIM 28
#define NUM_HUSH_DELIMITERS 1

struct sysEnv {
    char ARCH[50];
    char PATH[200];
    char SHELL[10];
    char PWD[250];
    char OLDPWD[250];
    char PAGER[20];
    char EDITOR[20];
    char HOME[100];
    char USER[40];
    char PS1[20];
    struct dV {
        char* name;
        int value; //for now, these will be integers.
    } DYNAMIC_VARIABLES[DYN_VAR_MAX];
} hushEnv = {"","/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin","hush",
             "/","/","less",
             "vi","","","<[H]> ", {"test", 27} };
             //The dynamics variables structure is not initialized properly. What
             //is the right way to do it?

typedef struct hE {
    long long int index;
    // some sort of timestamp
    char timestamp[TIMESTAMP_LIM]; 
    char cmdStr[CMD_STR_LIM];
    char cmdAST[CMD_REP_LIM][CMD_WORD_LIM][CMD_WORD_LEN_LIM];
    unsigned int cmdRep;
    unsigned int argc;
} historyEntry;

typedef struct cA {
    char *ca[CMD_WORD_LIM];
} commandArguments;

typedef struct jI {
    pid_t pid;
    char *cmdStr;  //the raw command.
    //what should a processed command be? It should be a list of commands which
    //all get executed. Or a single command which gets executed. Or a modification
    //of the shell's internal state (e.g. cd, pushd). Or a command which has its
    //arguments modified by the shell's state. Yeah, so a list of commands where
    //a command is a pre-processed 
    char cmd[CMD_REP_LIM][CMD_STR_LIM];        //the processed command.
                                           //this annoying duplication is needed because
                                           //different operating systems require different
                                           //exec system calls.  
    commandArguments cmdArgs[CMD_REP_LIM];
    char cmdAST[CMD_REP_LIM][CMD_WORD_LIM][CMD_WORD_LEN_LIM];
    unsigned int cmdRep;
    unsigned int argc;
    int isBackground;
    unsigned short int handleInternally;
} jobItem;

struct hS {
    int isInteractive;
    int isRunning;
    enum { okComputer, hushUnknownArch, hushUndefinedOption, hushCommandNotFound,
           hushInvalidExpansion, hushFilePathExpansionError, hushHistExpansionError,
           hushEnvironmentError, hushCmdWordOverflow
            } hushErrno;
    jobItem jobs[JOB_MAX];
    unsigned short int jobCount;
    historyEntry hushHist[HIST_SIZE];
    unsigned short int histCount;
    char dirStack[DIR_STACK_MAX];
    unsigned short int dirCount;
} hushState;

const char hushBuiltins[NUM_BUILTINS][BUILTIN_WORD_LEN] =  {"exit", "cd", "pwd", "pushd", "popd", "for", "repeat", "set", "fc", "hist", "source"};
const char hushHomeSymbol = '~';
const char envDeps[NUM_ENV_DEPS][BUILTIN_WORD_LEN] =  {"man"};

//Matters of Strategy:
//   I will not typedef structs which will have only one instantiation.
