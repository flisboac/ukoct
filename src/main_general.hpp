#ifndef MAIN_GENERAL_HPP_
#define MAIN_GENERAL_HPP_

#include "main.hpp"

enum EGeneralOption {
	OPT_GENERAL_NONE
    , OPT_GENERAL_HELP        // -h, --help Prints the help contents
	, OPT_GENERAL_VERSION     // --version Prints version information
	, OPT_GENERAL_SILENT      // -V      Activates silent mode
	, OPT_GENERAL_VERBOSE     // -v      Activates verbose mode
    , OPT_GENERAL_TIMELIMIT   // -t=NUM  Specifies the time limit, in seconds
    , OPT_GENERAL_OUTPUT      // -o=FILE Specifies the output
    , OPT_GENERAL_ELEMTYPE    // -e=TYPE Specify the element type to be used (float, double, half, etc)
    , OPT_GENERAL_EXECTYPE    // -x=SOLV Specify the solver to be used (CPU, OpenCL, etc)
    , OPT_GENERAL_OPERVARIANT // --variant=NAME Forces the solver to use a specific operator variant.
    , OPT_GENERAL_LISTFLAGS   // --list-flags Forces the solver to use a specific operator variant.

    , OPT_GENERAL_MIN_ = OPT_GENERAL_HELP
    , OPT_GENERAL_MAX_ = OPT_GENERAL_LISTFLAGS
};


extern const IOptionGroupCallback* const general_operationCallback;
extern const Option general_options[OPT_GENERAL_MAX_ + 1];


#endif /* MAIN_GENERAL_HPP_ */
