#include "main.hpp"
#include "main_details.hpp"
#include "ukoct.hpp"


const Option details_options[OPT_DETAILS_MAX_ + 1] = {
  { OPT_DETAILS_NONE        , OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, NULL                , "" }
, { OPT_DETAILS_CLSOURCE    , OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-source"       , "help" }
, { OPT_DETAILS_CLBINARY    , OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-binary"       , "help" }
, { OPT_DETAILS_CLSAVESOURCE, OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-output-source", "help" }
, { OPT_DETAILS_CLSAVEBINARY, OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-output-binary", "help" }
, { OPT_DETAILS_CLDEVID     , OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , "u4", NULL, "--cl-device-id"    , "help" }
, { OPT_DETAILS_CLPLATID    , OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , "u4", NULL, "--cl-platform-id"  , "help" }
, { OPT_DETAILS_CLPROGFLAGS , OPTG_DETAILS , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-program-flags", "help" }
};


class DetailsOptionGroupCallback : public IOptionGroupCallback {
public:
	inline EOptionGroup groupId() const {
		return OPTG_DETAILS;
	}


	bool operator()(ArgState& A, Operand* operand = NULL) const {
		bool keepgoing = true;

		if (operand == NULL) {
			switch(A.stage) {
			case STAGE_PREPARE: keepgoing = registerOptions(A); break;
			case STAGE_INIT: keepgoing = init(A); break;
			case STAGE_FINISH: finalize(A); break;
			}

		} else {
			switch(operand->stage) {
			//case STAGE_PARSE:  keepgoing = parse(A, *operand); break;
			}
		}

		return keepgoing;
	}


	void help(std::ostream& os, EHelpContents which) const {

	}


private:
	bool registerOptions(ArgState& A) const {
		bool keepgoing = true;

		for (int optionId = OPT_DETAILS_MIN_; optionId <= OPT_DETAILS_MAX_ && keepgoing; ++optionId) {
			const Option& option = details_options[optionId];
			keepgoing = A.registerOption(option);
		}

		return keepgoing;
	}


	bool init(ArgState& A) const {
		bool keepgoing = true;

		switch(A.implType) {
		case ukoct::IMPL_CPU:
		{

		}
		break;

		default:
			keepgoing = false;
			A.err() << "Invalid implementation type." << std::endl;
			A.exitCode = RETEARG;
			break;
		}

		return keepgoing;
	}


	void finalize(ArgState& A) const {

	}
};


static const DetailsOptionGroupCallback details_operationCallback_;
const IOptionGroupCallback* const details_operationCallback = &details_operationCallback_;
