#include "hushTypes.h"
    // hushTypes is not necessary except for the fact that it includes
    // string.h and I do not want to duplicate an include.
static struct option long_options[] = {
    {"PATH", required_argument, 0, 'p'},
    {"PWD", required_argument, 0, 'c' },
    {"PAGER", required_argument, 0, 'a' },
    {"EDITOR", required_argument, 0, 'e' },
    {"PS1", required_argument, 0, 'r' },
    {"nonInteractive", no_argument, 0, 'n'}, //interactive by default
    {0, 0, 0, 0}
};

struct optionSpec {
    char* knownOpts = "pcaern";
    int numKnownOpts = strlen(knownOpts);
    int optLen[knownOpts] = {2,2,2,2,2,1};
} hushOptSpec;
