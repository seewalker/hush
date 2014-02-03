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
              unsigned int lineLen;
              printf("%s", hushEnv.PS1);
              lineLen = getline(hushState.jobs[hushState.jobCount].cmdStr, cmdCap);
              errorFunnel(preprocessCmd(lineLen, hushState.jobs[hushState.jobCount].cmdStr));
              errorFunnel(doCmd(hushState.jobs[hushState.jobCount].cmd));
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
