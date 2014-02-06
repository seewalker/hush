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
    #ifdef __MACH__
    strcpy(hushEnv.ARCH, "Darwin");
    #endif
    if (strlen(hushEnv.ARCH) == 0) {
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
            case 'p' : 
                strcpy(hushEnv.PATH, optarg);
                break;
            case 'c' : 
                strcpy(hushEnv.PWD, optarg);
                break;
            case 'a' : 
                strcpy(hushEnv.PAGER, optarg);
                break;
            case 'e' : 
                strcpy(hushEnv.EDITOR, optarg);
                break;
            case 'r' : 
                strcpy(hushEnv.PS1, optarg);
                break;
            default : 
                hushState.hushErrno = hushUndefinedOption;
                break;
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
    hushState.jobs[hushState.jobCount].cmdRep = 1;
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
        case hushUnknownArch: 
            fprintf(stderr, "Warning, unrecognized architecture\n");
            break;
        case hushUndefinedOption: 
            fprintf(stderr, "Error, undefined option\n");
            exit(hushUndefinedOption);
        case hushEnvironmentError: 
            fprintf(stderr, "Error, malformed enviornment\n");
            exit(hushEnvironmentError);
        case hushHistExpansionError: 
            fprintf(stderr, "Warning, malformed history expansion\n");
            break;
        case hushFilePathExpansionError: 
            fprintf(stderr, "Warning, malformed filepath expansion\n");
            break;
        default: 
            fprintf(stderr, "Unclassified Error \n");
    }
}
//an auxilliary function for preprocessCmd
int expansions() {
    //path expansions
    return (hushState.hushErrno);
}

//an auxilliary function for preprocessCmd
int handleBuiltin(char* theBuiltin) {
    return (hushState.hushErrno);
}

// Side Effects: modifies hushState.jobs
// For description of int, see State.hushErrno
// setCmd
int preprocessCmd(int strLength, char* cmdStr) {
    char *tmpword;
    char *delim = malloc(NUM_HUSH_DELIMITERS + 1);
    unsigned int numTokens = 0;

    //Build cmdAST
    strcpy(delim, " ");
    tmpword = strtok(cmdStr, delim);
    while (tmpword != NULL) {
        strcpy(hushState.jobs[hushState.jobCount].cmdAST[0][numTokens], tmpword);
        ++numTokens;
        tmpword = strtok(NULL, delim);
    }

    //Operate on cmdAST
    for (int i = 0; i < numBuiltins; ++i) {
        if (strcmp(hushBuiltins[i], hushState.jobs[hushState.jobCount].cmdAST[0][0])) {
            errorFunnel(handleBuiltin(hushBuiltins[i]));
        }
    }
    //Eventually, I will handle envDeps
    errorFunnel(expansions());
    //Now we flatten the AST to get a sequence of commands
    for (int i = 0; i < hushState.jobs[hushState.jobCount].cmdRep; ++i) {
        for (int j = 0; j < numTokens; ++j) {
              strcat(hushState.jobs[hushState.jobCount].cmd[i], 
                     hushState.jobs[hushState.jobCount].cmdAST[i][j]);
              strcat(hushState.jobs[hushState.jobCount].cmd[i], " ");
              if (j > 0) {
              strcat(hushState.jobs[hushState.jobCount].cmdArgs[i],
                     hushState.jobs[hushState.jobCount].cmdAST[i][j]);
              strcat(hushState.jobs[hushState.jobCount].cmdArgs[i], " ");
              }
        }
    }
    return (hushState.hushErrno);
}
// A command 
int doCmd() {
    int forkRet = fork();
    //I may need to iterate through char** with a method other than strlen
    if (forkRet == 0) {  //child process branch
        printf("In the child process \n");
        for (int i = 0; i < hushState.jobs[hushState.jobCount].cmdRep; ++i) {
            printf("In the inner loop with hushEnv.ARCH %s\n", hushEnv.ARCH);
            if (strcmp(hushEnv.ARCH, "FreeBSD") || strcmp(hushEnv.ARCH, "Darwin")) {
                printf("Tried to run %s with argument %s in BSD branch\n",
                       hushState.jobs[hushState.jobCount].cmd[i],
                       hushState.jobs[hushState.jobCount].cmdArgs);
                execvP(hushState.jobs[hushState.jobCount].cmd[0],
                       hushEnv.PATH,
                       hushState.jobs[hushState.jobCount].cmdArgs);
            }
            else if (strcmp(hushEnv.ARCH, "Linux")) {
                printf("Tried to run %s with argument %s in Linux branch\n",
                       hushState.jobs[hushState.jobCount].cmd[i],
                       hushState.jobs[hushState.jobCount].cmdArgs);
                execvp(hushState.jobs[hushState.jobCount].cmd[i],
                       hushState.jobs[hushState.jobCount].cmdArgs);
            }
            else {
                fprintf(stderr, "Uncaught error: unrecognized architecture\n");
            }
        }
    }
    else if (forkRet > 0) { //parent process branch
        printf("In the parent process \n");
        if (! hushState.jobs[hushState.jobCount].isBackground) {
            while(1) { if (wait(NULL) == -1) { printf("looping"); break; } }
        }
    }
    else {
        fprintf(stderr, "Failed to fork in doCmd\n");
    }
    return(hushState.hushErrno);
}

int postprocessCmd() {
    //If 
}
