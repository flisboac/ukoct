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
		bool keepgoing = A.parseArgs(argc, argv);
		if (keepgoing) keepgoing = A.parseOperands();
		if (keepgoing) A.run();

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

	parser.overview = HELPTEXT;
	parser.syntax = USAGETEXT;
	parser.example = EXAMPLETEXT;
	parser.footer = FOOTERTEXT;

	// Sorry, my mistake.
	for (size_t i = 0; i < sizeof(options) / sizeof(Option); ++i) {
		const EOption opt = (EOption) i;
		const Option& desc = options[i];
		ez::ezOptionValidator* validator = desc.type ? new ez::ezOptionValidator(desc.type) : NULL;

		if (desc.shortFlag != NULL && desc.longFlag != NULL) {
			parser.add(
				desc.defaultVal
				, desc.required
				, desc.nargs
				, desc.argsep
				, desc.help
				, desc.shortFlag
				, desc.longFlag
				, validator
			);
		} else if (desc.longFlag != NULL) {
			parser.add(
				desc.defaultVal
				, desc.required
				, desc.nargs
				, desc.argsep
				, desc.help
				, desc.longFlag
				, validator
			);
		} else {
			ukoct_ASSERT(opt != OPT_NONE, "Invalid option found! Revise the source code!");
		}
	}

	return keepgoing;
}


bool ArgState::parseArgs(int argc, const char** argv) {
	bool keepgoing = true;
	size_t numActions = 0;
	std::vector<std::string> badOptions;
	std::vector<std::string> badArgs;
	parser.parse(argc, argv);

	if (!parser.gotRequired(badOptions)) {
		for(int i=0; i < badOptions.size(); ++i)
			err() << "Missing required option " << badOptions[i] << ".\n";
		err(false) << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		keepgoing = false;
	}

	if (keepgoing && !parser.gotExpected(badOptions)) {
		for(int i=0; i < badOptions.size(); ++i)
			err() << "Wrong arguments for option " << badOptions[i] << ".\n";
		err(false) << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		keepgoing = false;
	}

	if (keepgoing && !parser.gotValid(badOptions, badArgs)) {
		for(int i=0; i < badOptions.size(); ++i)
			err() << "Got invalid argument \"" << badArgs[i] << "\" for option " << badOptions[i] << "." << std::endl;
		err(false) << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		keepgoing = false;
	}

	if (keepgoing) {
		// Input
		if (!parser.lastArgs.empty()) {
			inputOperand.inputStream.fileName = *parser.lastArgs.back();
			std::cerr << inputOperand.inputStream.fileName << std::endl;
		}

		// Options
		// EDIT: Sorry, my bad :(
		for (int i = OPT_MIN_; i <= OPT_MAX_ && keepgoing; ++i) {
			EOption optionId = (EOption)i;
			const Option& option = options[optionId];
			const char* optionName = option.shortFlag ? option.shortFlag : option.longFlag;

			if (parser.isSet(optionName)) {
				Operand operand;
				operand.optionId = optionId;
				operand.ezGroup = parser.get(optionName);
				operand.parseIndex = operand.argGroup().parseIndex.back();

				if (option.action != ACT_NONE)
					numActions++;

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
					operand.argGroup().getString(outputOperand.outputStream.fileName);
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

				default: {
					std::vector<std::vector<std::string> > multiArgs;
					operand.argGroup().getMultiStrings(multiArgs);

					// If the option takes any arguments, or if giving an
					// argument is optional and was not given, ez will return
					// a non-empty multi-argument array[i][j] where each [i] is
					// the specific occurrence (that coincides in index with
					// the parseIndex vector in the OptionGroup), and [j] is
					// the split option (if any). array[i] should not be empty.
					if (!multiArgs.empty()) {
						for (size_t i = 0; i < multiArgs.size(); ++i) {
							Operand multiOperand;
							multiOperand.optionId = operand.optionId;
							multiOperand.parseIndex = operand.argGroup().parseIndex[i];
							multiOperand.ezGroup = operand.ezGroup;
							multiOperand.args = multiArgs[i];
							operands.push_back(operand);
						}

					} else {
						// In case the option doesn't take any arguments,
						// just insert each parseIndex to the operands list.
						// Those will be further sorted and then
						for (auto& parseIndex : operand.argGroup().parseIndex) {
							Operand operandOccurrence;
							operandOccurrence.optionId = operand.optionId;
							operandOccurrence.parseIndex = parseIndex;
							operandOccurrence.ezGroup = operand.ezGroup;
							operands.push_back(operandOccurrence);
						}
					}
					break;
				}
				}
			}
		}

	} else {
		exitCode = RETEARG;
	}

	if (numActions == 0) {
		err() << "Please specify at least one action." << std::endl;
		err(false) << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		exitCode = RETEARG;
		keepgoing = false;
	}

	if (keepgoing)
		stage = STAGE_PARSE;

	return keepgoing;
}


bool ArgState::parseOperands() {
	ukoct_ASSERT(stage == STAGE_PARSE, "parseOperands called and ArgState not in STAGE_PARSE.");
	bool keepgoing = true;
	std::sort(operands.begin(), operands.end());
	stage = STAGE_PREPARE;
	return keepgoing;
}


bool ArgState::parseInputOperand(Operand& operand) {
	return parseInputStream(operand);
}


bool ArgState::parseOutputOperand(Operand& operand) {
	return parseOutputStream(operand);
}


bool ArgState::parseInputStream(Operand& operand) {
	bool keepgoing = true;
	const Option& option = options[operand.optionId];

	if (operand.inputStream.fileName.empty()) {
		// Over-protecting?
		if (option.required) {
			err() << "Please specify an input file for option '" << option.longFlag << "'. " << std::endl;
			exitCode = RETERR;
			keepgoing = false;
		}

	} else if (operand.inputStream.fileName.compare("-") == 0) {
		std::string buf;
		operand.inputStream.stringStream = new std::stringstream();
		operand.inputStream.stream = operand.inputStream.stringStream;

		while (std::getline(std::cin, buf)) {
			*(operand.inputStream.stringStream) << buf << std::endl;
		}

	} else if (operand.inputStream.fileName.empty()) {
		operand.inputStream.fileStream = new std::ifstream();
		operand.inputStream.stream = operand.inputStream.fileStream;
		operand.inputStream.fileStream->open(operand.inputStream.fileName.c_str());

		if (!operand.inputStream.fileStream->is_open()) {
			err() << "Could not open input file \"" << operand.inputStream.fileName << "\"." << std::endl;
			exitCode = RETERR;
			keepgoing = false;
		}
	}

	return keepgoing;
}


bool ArgState::parseOutputStream(Operand& operand) {
	bool keepgoing = true;
	const Option& option = options[operand.optionId];

	if (operand.inputStream.fileName.empty()) {
		// Over-protecting?
		if (option.required) {
			err() << "Please specify an output file for option '" << options[operand.optionId].longFlag << "'."<< std::endl;
			exitCode = RETERR;
			keepgoing = false;
		}

	} else if (operand.outputStream.fileName.compare("-") == 0) {
		operand.outputStream.stream = &std::cout;

	} else {
		operand.outputStream.fileStream = new std::ofstream();
		operand.outputStream.stream = operand.outputStream.fileStream;

		operand.outputStream.fileStream->open(operand.outputStream.fileName.c_str());
		if (!operand.outputStream.fileStream->is_open()) {
			err() << "Could not open output file \"" << operand.outputStream.fileStream << "\"." << std::endl;
			exitCode = RETERR;
			keepgoing = false;
		}
	}

	return keepgoing;
}


void ArgState::run() {
	ukoct_ASSERT(stage == STAGE_PREPARE, "parseOperands called and ArgState not in STAGE_PREPARE.");
	bool keepgoing = true;
	const IOperationCallback* operationCallback = operationCallbacks[implType][elemType];
	bool parsedInputs = false;

	for (std::vector<Operand>::iterator it = operands.begin(); it != operands.end() && keepgoing; ++it) {
		const Option& option = options[it->optionId];
		EAction action = option.action;
		Operand& operand = *it;

		ukoct_ASSERT(action >= ACT_MIN_ && action <= ACT_MAX_, "Unknown action declared, check the source code!");
		switch(option.action) {
		case ACT_HELP:
			printHelp();
			exitCode = RETOK;
			keepgoing = false;
			break;

		case ACT_VERSION:
			printVersion();
			exitCode = RETOK;
			keepgoing = false;
			break;

		case ACT_CLPRINTSRC:
			if (operationCallback == NULL) {
				err() << "No implementation for element type '" << elemNames[elemType].name << "' on '" << implNames[implType].name << "." << std::endl;
				exitCode = RETNOTIMPL;
				keepgoing = false;

			} else {
				std::string source;
				operationCallback->source(source);
				std::cout << source << std::endl;
			}
			break;

		case ACT_OPER:
			if (operationCallback == NULL) {
				err() << "No implementation for element type '" << elemNames[elemType].name << "' on '" << implNames[implType].name << "." << std::endl;
				exitCode = RETNOTIMPL;
				keepgoing = false;

			} else {
				if (!parsedInputs) {
					parsedInputs = true;
					keepgoing = parseInputOperand(inputOperand) && parseOutputOperand(outputOperand);
				}
				keepgoing = parseInputOperand(operand);
				keepgoing = operationCallback->operator()(*this, *it);
			}
			break;
		}
	}

	if (keepgoing)
		stage = STAGE_RUN;

	for (std::vector<Operand>::iterator it = operands.begin(); it != operands.end() && keepgoing; ++it) {
		keepgoing = operationCallback->operator()(*this, *it);
	}

	if (keepgoing)
		stage = STAGE_FINISH;

	// The FINISH stage is left for the ArgState destructor to deal with.
}


void ArgState::printHelp() {
	std::string txt;
	parser.getUsage(txt);
	std::cerr << txt;
}


void ArgState::printVersion() {
	std::cerr << HELPTEXT << FOOTERTEXT << std::endl;
}


template <typename T> const T* findByName(const char* str, const T* list, size_t size) {
	const T* ret = NULL;
	for (size_t i = 0; i < size && ret == NULL; ++i, ++list)
		if (list->name != NULL && strcmp(str, list->name))
			ret = list;
	return ret;
}

