#include <stdlib.h>
#include <stdio.h>
#include "hushFunctions.c"
         //hushFunctions.c includes hushTypes.c; the include carries over.

/*USAGE:
    All options must be before the command 
*/
int main (int argc, char** argv) {
    errorFunnel(setEnv(argc, argv));
    errorFunnel(setState(argc, argv));
    errorFunnel(setCmd(argc, argv));
    if (hushState.isInteractive) {
        while(hushState.isRunning) {
              printf("%s", hushEnv.PS1);
              fgetline(stdin, hushState.jobs[jobCount].cmdStr, hushLimits.cmdLen);
              doCmd(hushState.jobs[jobCount].cmd);
              else { printf("Malformed Command, Error ID %i \n", hushState.hushErrno); }
        }
    }
    else { doCmd(hushState.jobs[jobCount].cmd); }
    return (hushState.hushErrno);
}
//Look, A main function without an assignment statement!
