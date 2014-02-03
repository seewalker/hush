#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>

#define jobCap 64
#define cmdCap 1000
#define dirCap 100
#define histCap 999999
//What is the meaning of all the struct words?

typedef int bool_t;
typedef int errVal;

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
    struct *dV {
        char* name;
        int value; //for now, these will be integers.
    }
} hushEnv = {"","/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin","hush",
             "/","/","less",
             "vi","","","<[H]> "};

typedef struct hE {
    long long int index;
    // some sort of timestamp
    struct timeval tv;
} historyEntry;

typedef struct jI {
    pid_t pid;
    char cmdStr[cmdCap];  //the raw command.
    //what should a processed command be? It should be a list of commands which
    //all get executed. Or a single command which gets executed. Or a modification
    //of the shell's internal state (e.g. cd, pushd). Or a command which has its
    //arguments modified by the shell's state. Yeah, so a list of commands where
    //a command is a pre-processed 
    char **cmd;           //the processed command.
    char ***cmdAST;
    bool_t isBackground;
} jobItem;

struct hS {
    bool_t isInteractive;
    bool_t isRunning;
    enum { okComputer, hushUnknownArch, hushUndefinedOption, hushCommandNotFound
           hushEnvironmentError } hushErrno;
    jobItem jobs[jobCap];
    unsigned short int jobCount;
    historyEntry hushHist[histCap];
    unsigned short int histCount;
    char dirStack[dirCap];
    unsigned short int dirCount;
} hushState;

const char *hushBuiltins[] =  {"cd", "pwd", "pushd", "popd", "for", "set", "fc", "hist"};
const char *envDeps[] =  {"man"};

//Matters of Strategy:
//   I will not typedef structs which will have only one instantiation.
