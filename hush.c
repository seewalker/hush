#include <stdio.h>
#include "hushFunctions.c"
         //hushFunctions.c includes hushTypes.c; the include carries over.

/*USAGE:
    All options must be before the command.
    All filepaths must be explicitly declared as filepaths. Either in ./filename form,
    ~/filename form or /filename form.
*/
int main (int argc, char** argv) {
    errorFunnel(setEnv(argc, argv));
    errorFunnel(setState(argc, argv));
    if (hushState.isInteractive) {
        while(hushState.isRunning) {
              printf("%s", hushEnv.PS1);
              fgetline(stdin, hushState.jobs[hushState.jobCount].cmdStr, cmdCap);
              errorFunnel(preprocessCmd(strlen(hushState.jobs[hushState.jobCount].cmdStr), 
                          hushState.jobs[hushState.jobCount].cmdStr));
              doCmd(hushState.jobs[jobCount].cmd);
              else { printf("Malformed Command, Error ID %i \n", hushState.hushErrno); }
              //The exit command will exit this loop.
        }
    }
    else { //hush is being run non-interactively with command specified with -n option.
        errorFunnel(preprocessCmd(strlen(hushState.jobs[hushState.jobCount].cmdStr), 
                    hushState.jobs[hushState.jobCount].cmdStr));
        doCmd(hushState.jobs[hushState.jobCount].cmd); 
        }
    return (hushState.hushErrno);
}
//Look, A main function without an assignment statement!
