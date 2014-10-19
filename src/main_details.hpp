#ifndef MAIN_DETAILS_HPP_
#define MAIN_DETAILS_HPP_

#include "main.hpp"

enum EDetailsOption {
	OPT_DETAILS_NONE
    , OPT_DETAILS_CLSOURCE
    , OPT_DETAILS_CLBINARY
    , OPT_DETAILS_CLSAVESOURCE
    , OPT_DETAILS_CLSAVEBINARY
    , OPT_DETAILS_CLDEVID
    , OPT_DETAILS_CLPLATID
    , OPT_DETAILS_CLPROGFLAGS

    , OPT_DETAILS_MIN_ = OPT_DETAILS_CLSOURCE
    , OPT_DETAILS_MAX_ = OPT_DETAILS_CLPROGFLAGS
};


extern const IOptionGroupCallback* const details_operationCallback;
extern const Option details_options[OPT_GENERAL_MAX_ + 1];


#endif /* MAIN_DETAILS_HPP_ */
