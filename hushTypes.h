#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>

//What is the meaning of all the struct words?
#define cmdStrLim 300
#define cmdRepLim 20
#define cmdWordLim 20
#define cmdWordLenLim 40
#define jobMax 32
#define histSize 1000
#define dirStackMax 16
#define dynVarMax 100
#define numBuiltins 10
#define NUM_ENV_DEPS 1
#define builtinWordLen 16
#define NUM_HUSH_DELIMITERS 1
#define TIMESTAMP_LIM 28

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
    } DYNAMIC_VARIABLES[dynVarMax];
} hushEnv = {"","/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin","hush",
             "/","/","less",
             "vi","","","<[H]> ", {"test", 27} };
             //The dynamics variables structure is not initialized properly. What
             //is the right way to do it?

typedef struct hE {
    long long int index;
    // some sort of timestamp
    char timestamp[TIMESTAMP_LIM]; 
    char cmdStr[cmdStrLim];
    char cmdAST[cmdRepLim][cmdWordLim][cmdWordLenLim];
    unsigned int cmdRep;
    unsigned int argc;
} historyEntry;

typedef struct cA {
    char *ca[cmdWordLim];
} commandArguments;

typedef struct jI {
    pid_t pid;
    char *cmdStr;  //the raw command.
    //what should a processed command be? It should be a list of commands which
    //all get executed. Or a single command which gets executed. Or a modification
    //of the shell's internal state (e.g. cd, pushd). Or a command which has its
    //arguments modified by the shell's state. Yeah, so a list of commands where
    //a command is a pre-processed 
    char cmd[cmdRepLim][cmdStrLim];        //the processed command.
                                           //this annoying duplication is needed because
                                           //different operating systems require different
                                           //exec system calls.  
    commandArguments cmdArgs[cmdRepLim];
    char cmdAST[cmdRepLim][cmdWordLim][cmdWordLenLim];
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
    jobItem jobs[jobMax];
    unsigned short int jobCount;
    historyEntry hushHist[histSize];
    unsigned short int histCount;
    char dirStack[dirStackMax];
    unsigned short int dirCount;
} hushState;

const char hushBuiltins[numBuiltins][builtinWordLen] =  {"exit", "cd", "pwd", "pushd", "popd", "for", "repeat", "set", "fc", "hist", "source"};
const char hushHomeSymbol = '~';
const char envDeps[NUM_ENV_DEPS][builtinWordLen] =  {"man"};

//Matters of Strategy:
//   I will not typedef structs which will have only one instantiation.
