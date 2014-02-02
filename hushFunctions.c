#include "hushTypes.h"
#include "hushOptions.h"
#include <unistd.h>
#include <getopt.h>
    // unistd.h provides the getopt feature

// Side Effects: modifies only hushEnv
// For description of errVal, see State.hushErrno
errVal setEnv(int argc, char** argv) {
    hushState.hushErrno = hushState.okComputer;
    // Architecture Stuff
    #ifdef __FreeBSD__
    strcpy(hushEnv.ARCH, "FreeBSD");
    #endif
    #ifdef __linux__
    strcpy(hushEnv.ARCH, "Linux");
    #endif
    if (hushEnv.ARCH != "Linux" && hushEnv.ARCH != "FreeBSD") {
        hushState.hushErrno = hushState.unknownArch;
    }
    
    // Establish default values. 
    if (getenv("PATH") != NULL) { strcpy(hushEnv.PATH, getenv("PATH")); }
    else { strcpy(hushEnv.PATH, "/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin"); }
    strcpy(hushEnv.SHELL, "hush");
    strcpy(hushEnv.PWD, getenv("PWD"));
    if (getenv("OLDPWD") != NULL) { strcpy(hushEnv.OLDPWD, getenv("OLDPWD")); }
    else { strcpy(hushEnv.OLDPWD, "/"; }
    if (getenv("PAGER") != NULL) { strcpy(hushEnv.PAGER, getenv("PAGER")); }
    else { strcpy(hushEnv.PAGER, "less"); }
    if (getenv("EDITOR") != NULL) { strcpy(hushEnv.EDITOR, getenv("EDITOR")); }
    else { strcopy(hushEnv.EDITOR, "vim"); }
    if (getenv("HOME") != NULL) { strcpy(hushEnv.HOME, getenv("HOME")); }
    else { hushState.hushErrno = hushState.environmentError; }
    if (getenv("USER") != NULL) { strcpy(hushEnv.USER, getenv("USER")); }
    else { hushState.hushErrno = hushState.environmentError; }
    strcpy(hushEnv.PS1, "<[H]>");

    // Override defaults based on options.
    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc,argv,"p:c:a:e:r:n",
                             long_options, &long_index)) != -1) {
        switch (opt) {
            case 'p' : strcpy(hushEnv.PATH, optarg);
            case 'c' : strcpy(hushEnv.PWD, optarg);
            case 'a' : strcpy(hushEnv.PAGER, optarg);
            case 'e' : strcpy(hushEnv.EDITOR, optarg);
            case 'r' : strcpy(hushEnv.PS1, optarg);
            default : hushState.hushErrno = hushState.undefinedOption;
        }
    }
    return (hushState.hushErrno);
}

// Side Effects: modifies only hushState
// For description of errVal, see State.hushErrno
errVal setState(int argc, char** argv) {
    hushState.isInteractive = 1;
    hushState.isRunning = 1;
    hushState.jobCount = 0;
    while ((opt = getopt_long(argc,argv,"p:c:a:e:r:n",
                             long_options, &long_index)) != -1) {
        switch (opt) {
            case 'n' :
                hushState.isInteractive = 0;
                hushState.isRunning = 0;
            default : hushState.hushErrno = hushState.undefinedOption;
        }
    }
    return (hushState.hushErrno);
}

void errorFunnel(errVal setEnvRes) {
    switch (setEnvRes) {
        case hushState.okComputer: return;
        case hushState.unknownArch: printf("Warning, unrecognized architecture\n");
        case hushState.undefinedOption: exit(hushState.undefinedOption);
        default: printf("Unclassified Error \n");
    }
}

// Side Effects: modifies hushState.jobs
// For description of errVal, see State.hushErrno
errVal setCmd(argc, argv, bool_t isStartup) {
    int arg = 1, argOffset = 0; //arg starts at 1 because argv[0] will be 'hush'.
    if (isStartup) {
        int numMatches = 0, discardMode = 1;
        while (discardMode) {
            for (int i = 0; i < hushOptSpec.numKnownOpts; ++i) {
                if (argv[arg] == strcat("-", hushOptSpec.knownOpts[i])) {
                    arg += hushOptSpec.optLen[i];
                    ++numMatches;
                }
            }
            if (numMatches == 0) { discardMode = 0; }
        }
    }
    //At this point, [argv[arg], argv[argc - 1]] is all the command.
    strcpy(hushState.jobs[hushState.jobCount].cmd.name, argv[arg]);
    argOffset = arg++;
    for (; arg < argc; ++arg) {
        strcpy(hushState.jobs[hushState.jobCount].cmd.optionsNargs[arg - argOffset], argv[arg]);
        if (arg == (argc - 1)) {
            if (argv[arg] == "&") { 
                hushState.jobs[hushState.jobCount].cmd.isBackground = 1;
                //I am choosing to increment the job counter later so that it is guarenteed
                //that the command beginned execution before another job is on the table.
            }
        }
    }
    return (hushState.hushErrno);
}

// A command 
void doCmd(command_t cmd) {
    pid_t forkRet = fork();
    for (
    if (forkRet == 0) {  //child process branch
        execv(sysEnv.PATH, hushState.jobs[hushState.jobCount].cmdStr);
    }
    else if (forkRet > 0) { //parent process branch
        if (! hushState.jobs[hushState.jobCount].cmd.isBackground) {
            while(1) { if (wait() == -1) { break; } }
        }
    }
    else {
        fprintf(stderr, "Failed to fork in doCmd\n");
    }
}
