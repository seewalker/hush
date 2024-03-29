#include "hushTypes.h"
#include <unistd.h>
#include <getopt.h>
    // hushTypes is not necessary except for the fact that it includes
    // string.h and I do not want to duplicate an include.
struct option *long_options = {
    {"PATH", required_argument, 0, 'p'},
    {"PWD", required_argument, 0, 'c' },
    {"PAGER", required_argument, 0, 'a' },
    {"EDITOR", required_argument, 0, 'e' },
    {"PS1", required_argument, 0, 'r' },
    {"nonInteractive", required_argument, 0, 'n'}, //interactive by default
    {0, 0, 0, 0}
};

