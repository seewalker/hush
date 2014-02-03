#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>

#define jobCap 64
#define cmdCap 1000
#define dirCap 100
#define histCap 999999
//What is the meaning of all the struct words?

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
    struct dV {
        char* name;
        int value; //for now, these will be integers.
    } *DYNAMIC_VARIABLES;
} hushEnv = {"","/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin","hush",
             "/","/","less",
             "vi","","","<[H]> ", {"test", 27} };
             //The dynamics variables structure is not initialized properly. What
             //is the right way to do it?
typedef struct hE {
    long long int index;
    // some sort of timestamp
    struct timeval tv;
    char *cmdStr;
} historyEntry;

typedef struct jI {
    pid_t pid;
    char *cmdStr;  //the raw command.
    //what should a processed command be? It should be a list of commands which
    //all get executed. Or a single command which gets executed. Or a modification
    //of the shell's internal state (e.g. cd, pushd). Or a command which has its
    //arguments modified by the shell's state. Yeah, so a list of commands where
    //a command is a pre-processed 
    char **cmd;           //the processed command.
    char ***cmdAST;
    int isBackground;
} jobItem;

struct hS {
    int isInteractive;
    int isRunning;
    enum { okComputer, hushUnknownArch, hushUndefinedOption, hushCommandNotFound,
           hushHistExpansionError, hushFilePathExpansionError,
           hushEnvironmentError } hushErrno;
    jobItem *jobs;
    unsigned short int jobCount;
    historyEntry *hushHist;
    unsigned short int histCount;
    char *dirStack;
    unsigned short int dirCount;
} hushState;

const char *hushBuiltins[] =  {"cd", "pwd", "pushd", "popd", "for", "set", "fc", "hist"};
const char *envDeps[] =  {"man"};

//Matters of Strategy:
//   I will not typedef structs which will have only one instantiation.
