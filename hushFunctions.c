#include "hushOptions.h"
    //hushOptions has already included hushTypes

// Side Effects: modifies only hushEnv
// For description of int, see State.hushErrno
int setState(int argc, char** argv) {
    hushState.isInteractive = 1;
    hushState.isRunning = 1;
    hushState.jobCount = 0;
    hushState.dirCount = 0;
    hushState.histCount = 0;
    hushState.jobs[hushState.jobCount].cmdRep = 1;
    hushState.jobs[hushState.jobCount].argc = 0;
    hushState.jobs[hushState.jobCount].handleInternally = 0;    
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
            case 'n' :
                hushState.isInteractive = 0;
                hushState.isRunning = 0;
                strcpy(hushState.jobs[hushState.jobCount].cmdStr, optarg);
                break;
            default : 
                hushState.hushErrno = hushUndefinedOption;
                break;
        }
    }
    return (hushState.hushErrno);
}


void errorFunnel(int setEnvRes) {
    switch (setEnvRes) {
        case okComputer: return;
        case hushUnknownArch: 
            fprintf(stderr, "Warning, unrecognized architecture\n");
            exit(hushUnknownArch);
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
    //unsigned int homeLen, argLen;
    //man page customization
    //if (strcmp(hushState.jobs[hushState.jobCount].cmdAST[0][0], "man") == 0) {
        //for (int i = (hushState.jobs[hushState.jobCount].argc - 1); i > 0; ++i) {
            //hushState.jobs[hushState.jobCount].cmdAST[0][i + 2] = hushState.jobs[hushState.jobCount].cmdAST[0][i]; 
        //}
        //strcpy(hushState.jobs[hushState.jobCount].cmdAST[0][1], "-P");
        //strcpy(hushState.jobs[hushState.jobCount].cmdAST[0][2], hushEnv.PAGER);
        //hushState.jobs[hushState.jobCount].argc += 2;
    //}
//
    ////path expansions
    //for (int i = 1; i < argc; ++i) {
        //homeSpace = strlen(hushEnv.HOME) - 1; //the minus one is there because the ~ character itself gets taken away.
        //argLen = strlen(hushState.jobs[hushState.jobCount].cmdAST[0][i]);
        //for (int j = 0; j < argLen; ++j) {
            //if (hushState.jobs[hushState.jobCount].cmdAST[0][i][j] == hushHomeSymbol) {
                //for (int k = (argLen - 1); k > j; --k) {
                    //if ( (k + homeLen) > cmdWordLenLim) {
                        //hushState.hushErrno = hushCmdWordOverflow;
                    //}
                    //hushState.jobs[hushState.jobCount].cmdAST[0][i][k + homeLen] = hushState.jobs[hushState.jobCount].cmdAST[0][i][k];
                    ////The line above shifts the rest of the argument over, I still need to fill in the $HOME part. 
                //}
            //}
        //}
    //}
//
    ////loop expansions
    //if (strcmp(hushState.jobs[hushState.jobCount].cmdAST[0][0], "repeat") == 0) {
        //for (int i = 1; i < atoi(hushState.jobs[hushState.jobCount].cmdAST[0][1]); ++i) {
            //for (int j = 0; j < hushState.jobs[hushState.jobCount].argc; ++j) {
                //strcpy(hushState.jobs[hushState.jobCount].cmdAST[i][j], hushState.jobs[hushState.jobCount].cmdAST[0][j]);
            //}
        //}
    //}
    ////example usage of for statement:   ``for i in {1..20} : <command>''
//
    ////dynamic variable expansions
//
    ////history expansions
    if (strcmp(hushState.jobs[hushState.jobCount].cmdAST[0][0], "hist") == 0) {
        unsigned int histIndex = atoi(hushState.jobs[hushState.jobCount].cmdAST[0][1]);
        for (int i = 0; i < hushState.hushHist[histIndex].cmdRep; ++i) {
            for (int j = 0; j < hushState.hushHist[histIndex].argc; ++j) {
                strcpy(hushState.jobs[hushState.jobCount].cmdAST[i][j], hushState.hushHist[histIndex].cmdAST[i][j]);
            }
        }
    }
        
    return (hushState.hushErrno);
}

// Side Effects: modifies hushState.jobs
// For description of int, see State.hushErrno
// setCmd
int preprocessCmd(int strLength, char* cmdStr) {
    char *tmpword;
    char *delim = malloc(NUM_HUSH_DELIMITERS + 1);
    hushState.jobs[hushState.jobCount].argc = 0;
    //Build cmdAST
    strcpy(delim, " ");
    tmpword = strtok(cmdStr, delim);
    while (tmpword != NULL) {
        strcpy(hushState.jobs[hushState.jobCount].cmdAST[0][hushState.jobs[hushState.jobCount].argc], tmpword);
        hushState.jobs[hushState.jobCount].argc += 1;
        tmpword = strtok(NULL, delim);
    }

    //Operate on cmdAST
    if (   strcmp("cd", hushState.jobs[hushState.jobCount].cmdAST[0][0]) == 0
        || strcmp("pwd", hushState.jobs[hushState.jobCount].cmdAST[0][0]) == 0
        || strcmp("exit", hushState.jobs[hushState.jobCount].cmdAST[0][0]) == 0
        || strcmp("pushd", hushState.jobs[hushState.jobCount].cmdAST[0][0]) == 0
        || strcmp("popd", hushState.jobs[hushState.jobCount].cmdAST[0][0]) == 0
        || strcmp("set", hushState.jobs[hushState.jobCount].cmdAST[0][0]) == 0) {
        hushState.jobs[hushState.jobCount].handleInternally = 1;
    }
    if ( strcmp("&", hushState.jobs[hushState.jobCount].cmdAST[0][hushState.jobs[hushState.jobCount].argc]) == 0) {
        hushState.jobs[hushState.jobCount].isBackground = 1;
        strcpy(hushState.jobs[hushState.jobCount].cmdAST[0][hushState.jobs[hushState.jobCount].argc], "\0");
        hushState.jobs[hushState.jobCount].argc -= 1;
    }
    //Note, it is important to handle builtins before doing expansions so that the expansions take effect for
    //all iterations of loop expansions.
    errorFunnel(expansions());
    //Now we flatten the AST to get a sequence of commands
    for (int i = 0; i < hushState.jobs[hushState.jobCount].cmdRep; ++i) {
        for (int j = 0; j < hushState.jobs[hushState.jobCount].argc; ++j) {
              strcat(hushState.jobs[hushState.jobCount].cmd[i], 
                     hushState.jobs[hushState.jobCount].cmdAST[i][j]);
              strcat(hushState.jobs[hushState.jobCount].cmd[i], " ");
              if (j > 0) {
                  hushState.jobs[hushState.jobCount].cmdArgs[i].ca[j - 1] = malloc(100);
                  strcpy(hushState.jobs[hushState.jobCount].cmdArgs[i].ca[j - 1],
                         hushState.jobs[hushState.jobCount].cmdAST[i][j]);
              }
        }
    }
    return (hushState.hushErrno);
}
// A command 
int doCmd(unsigned int isInternal) {
    if (!isInternal) {
        int forkRet = fork();
        //I may need to iterate through char** with a method other than strlen
        if (forkRet == 0) {  //child process branch
            for (int i = 0; i < hushState.jobs[hushState.jobCount].cmdRep; ++i) {
                printf("In the inner loop with hushEnv.ARCH %s\n", hushEnv.ARCH);
                if (strcmp(hushEnv.ARCH, "FreeBSD") == 0 || 
                    strcmp(hushEnv.ARCH, "Darwin") == 0) {
                    execvP(hushState.jobs[hushState.jobCount].cmdAST[i][0],
                           hushEnv.PATH,
                           hushState.jobs[hushState.jobCount].cmdArgs[i].ca);
                }
                else if (strcmp(hushEnv.ARCH, "Linux") == 0) {
                    execvp(hushState.jobs[hushState.jobCount].cmdAST[i][0],
                           hushState.jobs[hushState.jobCount].cmdArgs[i].ca);
                }
                else {
                    fprintf(stderr, "Uncaught error: unrecognized architecture\n");
                }
            }
        }
        else if (forkRet > 0) { //parent process branch
            if (! hushState.jobs[hushState.jobCount].isBackground) {
                while(1) { if (wait(NULL) == -1) { printf("looping"); break; } }
            }
        }
        else {
            fprintf(stderr, "Failed to fork in doCmd\n");
        }
    }
    else { //internal command
        if (strcmp(hushState.jobs[hushState.jobCount].cmdAST[0][0], "exit") == 0) {
            exit(0);
        }
    }
    return(hushState.hushErrno);
}

int postprocessCmd() {
    //save  a histEntry struct.
    time_t t;
    t = time(NULL);
    strcpy( hushState.hushHist[hushState.histCount].timestamp, asctime(localtime(&t)));
    strcpy( hushState.hushHist[hushState.histCount].cmdStr, hushState.jobs[hushState.jobCount].cmdStr);
    for (int i = 0; i < hushState.jobs[hushState.jobCount].cmdRep; ++i) {
        for (int j = 0; j < hushState.jobs[hushState.jobCount].argc; ++j) {
            strcpy(hushState.hushHist[hushState.histCount].cmdAST[i][j],
                   hushState.jobs[hushState.jobCount].cmdAST[i][j]);
        }
    }
    hushState.hushHist[hushState.histCount].cmdRep = hushState.jobs[hushState.jobCount].cmdRep;
    hushState.hushHist[hushState.histCount].argc = hushState.jobs[hushState.jobCount].argc;
    hushState.histCount += 1;
    return(hushState.hushErrno);
}
