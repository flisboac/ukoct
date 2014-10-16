#include "main.hpp"
#include "main_general.hpp"


const Option general_options[OPT_GENERAL_MAX_ + 1] = {
	//  id              , group       , oper                        , reqd , na, sep , defv    , vald, shrt, long                , help
	  { OPT_GENERAL_NONE        , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, NULL                , "The input file. Btw, this help content won't appear..." }

	// General Options
	, { OPT_GENERAL_HELP        , OPTG_GENERAL, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, "-h", "--help"            , "Displays help content." }
	, { OPT_GENERAL_VERSION     , OPTG_GENERAL, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, NULL, "--version"         , "Displays version information." }
	, { OPT_GENERAL_SILENT      , OPTG_GENERAL, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, "-V", "--silent"          , "Disables all messages." }
	, { OPT_GENERAL_VERBOSE     , OPTG_GENERAL, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, "-v", "--verbose"         , "Displays additional information. Use -vv for debug mode." }
	, { OPT_GENERAL_TIMELIMIT   , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', "0"     , "u4", "-t", "--time-limit"      , "Sets up a time limit for operation execution. Given in milliseconds." }
	, { OPT_GENERAL_OUTPUT      , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', "-"     , NULL, "-o", "--output-file"     , "Specifies an output file." }
	, { OPT_GENERAL_ELEMTYPE    , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', "float" , NULL, "-e", "--elem-type"       , "Specifies the element type to be used. The available types are dependant on the implementation type. The default type is 'float'." }
	, { OPT_GENERAL_EXECTYPE    , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', "opencl", NULL, "-x", "--exec-type"       , "Specifies the implementation to be used. The default implementation is 'opencl'. The available implementations are (" ukoct_AVAILABLE_IMPL ")" }
	, { OPT_GENERAL_OPERVARIANT , OPTG_GENERAL, ukoct::OPER_NONE            , false,  2,  ':', ""      , NULL, NULL, "--variant"         , "Specifies a variant for a given operator." }
	, { OPT_GENERAL_LISTFLAGS   , OPTG_GENERAL, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, NULL, "--list-flags"      , "Lists possible configurations and values for all flags." }
};


class GeneralOptionGroupCallback : public IOptionGroupCallback {
public:
	inline EOptionGroup groupId() const {
		return OPTG_GENERAL;
	}


	bool operator()(ArgState& A, Operand* operand = NULL) const {
		bool keepgoing = true;
		return keepgoing;
	}


	void help(std::ostream& os, EHelpContents which) const {

	}
};


static const GeneralOptionGroupCallback general_operationCallback_;
const IOptionGroupCallback* const general_operationCallback = &general_operationCallback_;
