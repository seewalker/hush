#include "hushOptions.h"
    //hushOptions has already included hushTypes

// Side Effects: modifies only hushEnv
// For description of int, see State.hushErrno
int setEnv(int argc, char** argv) {
    hushState.hushErrno = okComputer;
    // Architecture Stuff
    #ifdef __FreeBSD__
    strcpy(hushEnv.ARCH, "FreeBSD");
    #endif
    #ifdef __linux__
    strcpy(hushEnv.ARCH, "Linux");
    #endif
    if (strcmp(hushEnv.ARCH, "Linux") && strcmp(hushEnv.ARCH , "FreeBSD")) {
        hushState.hushErrno = hushUnknownArch;
    }
    
    // Establish default values. 
    if (getenv("PATH") != NULL) { strcpy(hushEnv.PATH, getenv("PATH")); }
    strcpy(hushEnv.SHELL, "hush");
    strcpy(hushEnv.PWD, getenv("PWD"));
    if (getenv("OLDPWD") != NULL) { strcpy(hushEnv.OLDPWD, getenv("OLDPWD")); }
    else { strcpy(hushEnv.OLDPWD, "/"); }
    if (getenv("PAGER") != NULL) { strcpy(hushEnv.PAGER, getenv("PAGER")); }
    if (getenv("EDITOR") != NULL) { strcpy(hushEnv.EDITOR, getenv("EDITOR")); }
    if (getenv("HOME") != NULL) { strcpy(hushEnv.HOME, getenv("HOME")); }
    else { hushState.hushErrno = hushEnvironmentError; }
    if (getenv("USER") != NULL) { strcpy(hushEnv.USER, getenv("USER")); }
    else { hushState.hushErrno = hushEnvironmentError; }
    // Override defaults based on options.
    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc,argv,"p:c:a:e:r:n:",
                             long_options, &long_index)) != -1) {
        switch (opt) {
            case 'p' : strcpy(hushEnv.PATH, optarg);
            case 'c' : strcpy(hushEnv.PWD, optarg);
            case 'a' : strcpy(hushEnv.PAGER, optarg);
            case 'e' : strcpy(hushEnv.EDITOR, optarg);
            case 'r' : strcpy(hushEnv.PS1, optarg);
            default : hushState.hushErrno = hushUndefinedOption;
        }
    }
    return (hushState.hushErrno);
}

// Side Effects: modifies only hushState
// For description of int, see State.hushErrno
int setState(int argc, char** argv) {
    int opt = 0, long_index = 0;
    hushState.isInteractive = 1;
    hushState.isRunning = 1;
    hushState.jobCount = 0;
    hushState.dirCount = 0;
    hushState.histCount = 0;
    while ((opt = getopt_long(argc,argv,"p:c:a:e:r:n:",
                             long_options, &long_index)) != -1) {
        if (opt == 'n') {
            hushState.isInteractive = 0;
            hushState.isRunning = 0;
            strcpy(hushState.jobs[hushState.jobCount].cmdStr, optarg);
        }
    }
    return (hushState.hushErrno);
}

void errorFunnel(int setEnvRes) {
    switch (setEnvRes) {
        case okComputer: return;
        case hushUnknownArch: fprintf(stderr, "Warning, unrecognized architecture\n");
        case hushUndefinedOption: exit(hushUndefinedOption);
        case hushEnvironmentError: exit(hushEnvironmentError);
        case hushHistExpansionError: fprintf(stderr, "Malformed history Expansion\n");
        case hushFilePathExpansionError: fprintf(stderr, "Malformed filepath Expansion\n");
        default: fprintf(stderr, "Unclassified Error \n");
    }
}
//an auxilliary function for preprocessCmd
int expansions() {
    //pathname expansions

    //history expansions

    return (hushState.hushErrno);
}

//an auxilliary function for preprocessCmd
int handleBuiltin(char* theBuiltin) {
    return (hushState.hushErrno);
}

// Side Effects: modifies hushState.jobs
// For description of int, see State.hushErrno
// setCmd
int preprocessCmd(int strLength, char* str) {
    char *word;
    int j = 0;
    //Build cmdAST
    for (int i = 0; i < strLength; ++i) {
        if (str[i] == ' ') {
            while (str[i] == ' ') {
                str[i] = str[i + 1];
            }
            strcpy(hushState.jobs[hushState.jobCount].cmdAST[0][j], word);
            ++j;
        }
        else {
            word = strcat(word, &(str[i])); //append the current character to the current word.
        }
    }
    //Operate on cmdAST
    for (int i = 0; i < strlen(hushBuiltins); ++i) {
        if (strcmp(hushBuiltins[i], hushState.jobs[hushState.jobCount].cmdAST[0][0])) {
            hushState.hushErrno = handleBuiltin(hushBuiltins[i]);
            if (hushState.hushErrno != okComputer) {
                errorFunnel(hushState.hushErrno);
            }
        }
    }
    errorFunnel(expansions()); 
    //Now we flatten the AST to get a sequence of commands
    for (int i = 0; i < strlen(hushState.jobs[hushState.jobCount].cmdAST); ++i) {
        for (int j = 0; j < strlen(hushState.jobs[hushState.jobCount].cmdAST[i]); ++j) {
            strcat(hushState.jobs[hushState.jobCount].cmd[i],
                   hushState.jobs[hushState.jobCount].cmdAST[i][j]);
        }
    }
    return (hushState.hushErrno);
}
// A command 
int doCmd(char **cmd) {
    pid_t forkRet = fork();
    //I may need to iterate through char** with a method other than strlen
    if (forkRet == 0) {  //child process branch
        for (int i = 0; i < strlen(cmd); ++i) {
        execv(hushEnv.PATH, hushState.jobs[hushState.jobCount].cmd[i]);
        }
    }
    else if (forkRet > 0) { //parent process branch
        if (! hushState.jobs[hushState.jobCount].isBackground) {
            while(1) { if (wait(NULL) == -1) { break; } }
        }
    }
    else {
        fprintf(stderr, "Failed to fork in doCmd\n");
    }
    return(hushState.hushErrno);
}
