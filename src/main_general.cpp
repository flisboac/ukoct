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
	, { OPT_GENERAL_PRINTTIMINGS, OPTG_GENERAL, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, "-p", "--print-timings"   , "Prints timing and performance information.." }
	, { OPT_GENERAL_OUTPUT      , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', "-"     , NULL, "-o", "--output-file"     , "Specifies an output file." }
	, { OPT_GENERAL_ELEMTYPE    , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', "double", NULL, "-e", "--elem-type"       , "Specifies the element type to be used. The available types are dependant on the implementation type. The default type is 'float'." }
	, { OPT_GENERAL_EXECTYPE    , OPTG_GENERAL, ukoct::OPER_NONE            , false,  1, '\0', "cpu"   , NULL, "-x", "--exec-type"       , "Specifies the implementation to be used. The default implementation is 'opencl'. The available implementations are (" ukoct_AVAILABLE_IMPL ")" }
	, { OPT_GENERAL_OPERVARIANT , OPTG_GENERAL, ukoct::OPER_NONE            , false,  2,  ':', ""      , NULL, "-O", "--variant"         , "Specifies a variant for a given operator." }
	, { OPT_GENERAL_LISTFLAGS   , OPTG_GENERAL, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, NULL, "--list-flags"      , "Lists possible configurations and values for all flags." }
};


static void splitString(std::vector<std::string> &elems, const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


class GeneralOptionGroupCallback : public IOptionGroupCallback {
public:
	inline EOptionGroup groupId() const {
		return OPTG_GENERAL;
	}


	bool operator()(ArgState& A, Operand* operand = NULL) const {
		bool keepgoing = true;

		if (operand == NULL) {
			switch(A.stage) {
			case STAGE_PREPARE: keepgoing = registerOptions(A); break;
			}

		} else {
			switch(operand->stage) {
			case STAGE_PARSE:  keepgoing = parse(A, *operand); break;
			}
		}

		return keepgoing;
	}


	void help(std::ostream& os, EHelpContents which) const {

	}


private:
	bool registerOptions(ArgState& A) const {
		bool keepgoing = true;

		for (int optionId = OPT_GENERAL_MIN_; optionId <= OPT_GENERAL_MAX_ && keepgoing; ++optionId) {
			const Option& option = general_options[optionId];
			keepgoing = A.registerOption(option);
		}

		return keepgoing;
	}


	bool parse(ArgState& A, Operand& operand) const {
		bool keepgoing = true;

		switch (operand.option->id) {
		case OPT_GENERAL_HELP:
		{
			A.printHelp(A.err(false));
			keepgoing = false;
		}
		break;

		case OPT_GENERAL_VERSION:
		{
			A.printVersion(A.err(false));
			keepgoing = false;
		}
		break;

		case OPT_GENERAL_SILENT:
		{
			A.verbose = VERB_SILENT;
		}
		break;

		case OPT_GENERAL_VERBOSE:
		{
			A.verbose = VERB_VERBOSE;
			if (operand.ezGroup->parseIndex.size() > 1) {
				A.verbose = VERB_DEBUG;
			}
		}
		break;

		case OPT_GENERAL_TIMELIMIT:
		{
			operand.ezGroup->getInt(A.maxTime);
		}
		break;

		case OPT_GENERAL_PRINTTIMINGS:
		{
			A.printTimings = true;
		}
		break;

		case OPT_GENERAL_OUTPUT:
		{
			operand.ezGroup->getString(A.outputOperand.outputStream.fileName);
		}
		break;

		case OPT_GENERAL_ELEMTYPE:
		{
			std::string typeName;
			operand.ezGroup->getString(typeName);
			const TypeName* entry = findByName(typeName.c_str(), elemNames, ukoct::ELEM_MAX_ + 1);
			if (entry == NULL) {
				A.err() << "'" << typeName << "' is not a valid element typename." << std::endl
						<< "Check `" << ukoct_NAME << " --help` for more info.";
				A.exitCode = RETEARG;
				keepgoing = false;
			} else {
				A.elemType = (ukoct::EElemType) entry->id;
			}
		}
		break;

		case OPT_GENERAL_EXECTYPE:
		{
			std::string typeName;
			operand.ezGroup->getString(typeName);
			const TypeName* entry = findByName(typeName.c_str(), implNames, ukoct::IMPL_MAX_ + 1);
			if (entry == NULL) {
				A.err() << "'" << typeName << "' is not a valid implementation name." << std::endl
						<< "Check `" << ukoct_NAME << " --help` for more info.";
				A.exitCode = RETEARG;
				keepgoing = false;
			} else {
				A.implType = (ukoct::EImplementation) entry->id;
			}
		}
		break;

		case OPT_GENERAL_OPERVARIANT:
		{
			ukoct_ASSERT(operand.args.size() != 2, "Parser did not return 2 arguments for variant option.");
			const TypeName* operEntry = findByName(operand.args[0].c_str(), operNames, ukoct::OPER_MAX_ + 1);

			if (operEntry == NULL) {
				A.err() << "'" << operand.args[0] << "' is not a valid element typename." << std::endl
						<< "Check `" << ukoct_NAME << " --help` for more info.";
				A.exitCode = RETEARG;
				keepgoing = false;
			}

			if (keepgoing) {
				std::vector<std::string> variantNames;
				ukoct::EOperation operation = (ukoct::EOperation) operEntry->id;
				splitString(variantNames, operand.args[1], ',');

				for (auto& variantName : variantNames) {
					if (variantName.empty()) continue;
					const VariantDefinition* variantEntry = findByName(variantName.c_str(), variantDefinitions, variantDefinitions_sz);

					if (variantEntry == NULL) {
						A.err() << "'" << variantName << "' is not a valid operator variant name." << std::endl
								<< "Check `" << ukoct_NAME << " --help` for more info.";
						A.exitCode = RETEARG;
						keepgoing = false;
						break;

					} else {
						auto it = A.variants.find(operation);

						if (it == A.variants.end()) {
							A.variants[operation] = variantEntry->details;

						} else {
							it->second.group(variantEntry->group, variantEntry->details);
						}
					}
				}
			}
		}
		break;

		case OPT_GENERAL_LISTFLAGS:
		{
			keepgoing = false;

			const char* sep = "";
			A.warn(false) << "* Available element types:" << std::endl;
			for (int elemTypeId = ukoct::ELEM_MIN_; elemTypeId <= ukoct::ELEM_MAX_; ++elemTypeId) {
				const TypeName& entry = elemNames[elemTypeId];
				if (entry.name != NULL) {
					A.warn(false) << sep << entry.name;
					sep = ", ";
				}
			}

			sep = "";
			A.warn(false) << std::endl << std::endl << "* Available implementations:" << std::endl;
			for (int elemTypeId = ukoct::IMPL_MIN_; elemTypeId <= ukoct::IMPL_MAX_; ++elemTypeId) {
				const TypeName& entry = implNames[elemTypeId];
				if (entry.name != NULL) {
					A.warn(false) << sep << entry.name;
					sep = ", ";
				}
			}

			A.warn(false) << std::endl << std::endl << "* Available operators: " << std::endl;
			for (int elemTypeId = ukoct::OPER_MIN_; elemTypeId <= ukoct::OPER_MAX_; ++elemTypeId) {
				const TypeName& entry = operNames[elemTypeId];
				if (entry.name != NULL) {
					A.warn(false) << entry.name << std::endl;
				}
			}

			// TODO Printing of variant help contents
			A.warn(false) << std::endl << "* Available variant names: " << std::endl;
			for (int id = 0; id < variantDefinitions_sz; ++id) {
				const VariantDefinition& entry = variantDefinitions[id];
				if (entry.name != NULL) {
					A.warn(false) << entry.name << std::endl;
				}
			}

			A.warn(false) << std::endl;
		}
		break;

		}

		return keepgoing;
	}
};


static const GeneralOptionGroupCallback general_operationCallback_;
const IOptionGroupCallback* const general_operationCallback = &general_operationCallback_;
