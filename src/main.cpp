#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include "plas.hpp"
#include "ukoct.hpp"
#include "main.hpp"

#include <algorithm>

template <typename T> const T* findByName(const char* str, const T* list, size_t size);

int main(int argc, const char** argv) {
	ArgState A;

	// Adds all options and sets parser up
	if (!A.initialize()) {
		return A.exitCode;
	}

	try {
		// Parse arguments
		if (
			A.parseArgs(argc, argv)
			&& A.parseOperands()
		) {
			A.run();
		}

	} catch (ukoct::Error& e) {
		A.err() << "UKOCT EXCEPTION " << e.code() << ": " << e.what() << std::endl;
		A.exitCode = RETERR;

	} catch (cl::Error& e) {
		A.err() << "OPENCL EXCEPTION " << e.err() << ": " << e.what() << std::endl;
		A.exitCode = RETERR;
	}

	return A.exitCode;
}


bool ArgState::initialize() {
	bool keepgoing = true;
	const Option* desc = options;

	parser.overview = HELPTEXT;
	parser.syntax = USAGETEXT;
	parser.example = EXAMPLETEXT;
	parser.footer = FOOTERTEXT;

	// This is dangerous, I tell you.
	// Why? Because in ez the options are stored solely by name, and an index
	// is generated for each newly added entry. ez was not necessarily done
	// for direct ID access, and in the case of ukoct, the ID of a group in ez
	// must strictly coincide with the order in which its correspondent option
	// was inserted, in order for all the other CLI implementation's
	// assumptions to work. Well, that is the case, but if I ever get some
	// spare time in the future...
	// TODO Substitute (or implement my own) option parser in the future.
	while (desc != NULL) {
		ez::ezOptionValidator* validator = desc->type ? new ez::ezOptionValidator(desc->type) : NULL;

		if (desc->shortFlag && desc->longFlag) {
			parser.add(
				desc->defaultVal
				, desc->required
				, desc->nargs
				, desc->argsep
				, desc->help
				, desc->shortFlag
				, desc->longFlag
				, validator
			);
		} else {
			parser.add(
				desc->defaultVal
				, desc->required
				, desc->nargs
				, desc->argsep
				, desc->help
				, desc->shortFlag ? desc->shortFlag : desc->longFlag
				, validator
			);
		}
		desc++;
	}

	return keepgoing;
}


bool ArgState::parseArgs(int argc, const char** argv) {
	bool keepgoing = true;
	std::vector<std::string> badOptions;
	std::vector<std::string> badArgs;
	parser.parse(argc, argv);

	if (!parser.gotRequired(badOptions)) {
		for(int i=0; i < badOptions.size(); ++i)
			err() << "Missing required option " << badOptions[i] << ".\n";
		err() << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		keepgoing = false;
	}

	if (keepgoing && !parser.gotExpected(badOptions)) {
		for(int i=0; i < badOptions.size(); ++i)
			err() << "Wrong arguments for option " << badOptions[i] << ".\n";
		err() << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		keepgoing = false;
	}

	if (keepgoing && !parser.gotValid(badOptions, badArgs)) {
		for(int i=0; i < badOptions.size(); ++i)
			err() << "Got invalid argument \"" << badArgs[i] << "\" for option " << badOptions[i] << "." << std::endl;
		err() << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		keepgoing = false;
	}

	if (keepgoing) {
		// Input
		if (!parser.lastArgs.empty()) {
			input.fileName = *parser.lastArgs[0];
		}

		// Options
		// This argument parser has the annoying limitation of not being able
		// to keep multiple occurrences of the same option. This makes it
		// impossible for users to do things like
		// `ukoct input.plas +cl +pushc a+b-5 +pushc a+d+2 +cl
		for (int i = OPT_MIN_; i <= OPT_MAX_ && keepgoing; ++i) {
			EOption option = (EOption)i;
			const char* optionName = options[option].shortFlag ? options[option].shortFlag : options[option].longFlag;

			if (parser.isSet(optionName)) {
				Operand operand;
				operand.optionId = option;
				operand.ezGroup = parser.get(optionName);
				operand.parseIndex = operand.argGroup().parseIndex.back();

				switch(operand.optionId) {
				case OPT_SILENT:
					verbose = VERB_SILENT;
					break;

				case OPT_VERBOSE:
					verbose = VERB_VERBOSE;
					break;

				case OPT_TIMELIMIT:
					operand.argGroup().getInt(maxTime);
					break;

				case OPT_OUTPUT:
					operand.argGroup().getString(output.fileName);
					break;

				case OPT_ELEMTYPE: {
					std::string name;
					operand.argGroup().getString(name);
					const TypeName* info = findByName(name.c_str(), elemNames, sizeof(elemNames) / sizeof(TypeName));
					if (info == NULL) {
						err() << "Invalid element typename " << name << std::endl;
						exitCode = RETEARG;
						keepgoing = false;
					} else
						elemType = (ukoct::EElemType) info->id;
					break;
				}

				case OPT_EXECTYPE: {
					std::string name;
					operand.argGroup().getString(name);
					const TypeName* info = findByName(name.c_str(), implNames, sizeof(implNames) / sizeof(implNames));
					if (info == NULL) {
						err() << "Invalid implementation name " << name << std::endl;
						exitCode = RETEARG;
						keepgoing = false;
					} else
						implType = (ukoct::EImplementation) info->id;
					break;
				}

				case OPT_OPERVARIANT: {
					// First, get all declared variants
					std::vector<std::vector<std::string> > declaredVariants;
					operand.argGroup().getMultiStrings(declaredVariants);

					// Iterate over all its occurrences
					for (std::vector<std::vector<std::string> >::iterator it = declaredVariants.begin(); it != declaredVariants.end() && keepgoing; ++it) {
						std::vector<std::string>& declaredVariantPair = *it;

						// I could remove this if I was not suspect of ez...
						if (declaredVariantPair.size() != 2) {
							err() << "Variant arguments requires 2 arguments, instead " << declaredVariantPair.size() << " has been given." << std::endl;
							exitCode = RETEARG;
							keepgoing = false;
						}

						if (keepgoing) {
							// Check if the operator and optimization names are
							// correct
							const TypeName* name = findByName(declaredVariantPair[0].c_str(), operNames, sizeof(operNames) / sizeof(TypeName));
							const OperDetailValue* val = findByName(declaredVariantPair[1].c_str(), operDetailValues, sizeof(operDetailValues) / sizeof(OperDetailValue));

							if (name == NULL) {
								err() << "Non-existent operator " << declaredVariantPair[0] << " given in variant option." << std::endl;
								exitCode = RETEARG;
								keepgoing = false;
							}

							if (val == NULL) {
								err() << "Non-existent optimization " << declaredVariantPair[1] << " given in variant option." << std::endl;
								exitCode = RETEARG;
								keepgoing = false;
							}

							if (keepgoing) {
								// Ensure that only one option is set per
								// detail group and operation
								ukoct::EOperation operation = (ukoct::EOperation)name->id;
								ukoct::OperationDetails details = val->details;
								ukoct::OperationDetails detailsGroup = val->group;
								std::map<ukoct::EOperation, OperDetails>::iterator entry = variants.find(operation);
								if (entry == variants.end())
									entry = variants.insert(entry, std::make_pair(operation, OperDetails()));
								entry->second.details.group(detailsGroup, details);
							}
						}
					}
					break;
				}
				case OPT_CLSOURCE:
					warn() << "--cl-source is not implemented yet, sorry!" << std::endl;
					break;

				case OPT_CLBINARY:
					warn() << "--cl-binary is not implemented yet, sorry!" << std::endl;
					break;

				case OPT_CLDEVID:
					warn() << "--cl-device-id is not implemented yet, sorry!" << std::endl;
					break;

				case OPT_CLPLATID:
					warn() << "--cl-platform-id is not implemented yet, sorry!" << std::endl;
					break;

				case OPT_CLPROGFLAGS:
					warn() << "--cl-program-flags is not implemented yet, sorry!" << std::endl;
					break;

				default:
					operands.push_back(operand);
					break;
				}
			}
		}

	} else {
		exitCode = RETEARG;
	}

	return keepgoing;
}


bool ArgState::parseOperands() {
	bool keepgoing = true;
	std::sort(operands.begin(), operands.end());
	keepgoing = parseOperand(input) && parseOperand(output);
	for (std::vector<Operand>::iterator it = operands.begin(); it != operands.end() && keepgoing; ++it)
		keepgoing = parseOperand(*it);
	return keepgoing;
}


bool ArgState::parseOperand(Operand& operand) {
	bool keepgoing = true;
	return keepgoing;
}

void ArgState::run() {

}


void ArgState::printHelp() {
	std::string txt;
	parser.getUsage(txt);
	err() << txt;
}


void ArgState::printVersion() {
	err() << HELPTEXT << FOOTERTEXT << std::endl;
}


template <typename T> const T* findByName(const char* str, const T* list, size_t size) {
	const T* ret = NULL;
	for (size_t i = 0; i < size && ret == NULL; ++i, ++list)
		if (list->name != NULL && strcmp(str, list->name))
			ret = list;
	return ret;
}

