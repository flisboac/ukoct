#include <algorithm>

#include "main.hpp"

int main(int argc, const char** argv) {
	ArgState A;

	if (A.initialize()) {
		A.run(argc, argv);
		A.finalize();
	}

	return A.exitCode;
}


ArgState::~ArgState() {
	if (stage != STAGE_FINISH) {
		finalize();
	}
}


bool ArgState::initialize() {
	bool keepgoing = true;
	parser.overview = HELPTEXT;
	parser.syntax = USAGETEXT;
	parser.example = EXAMPLETEXT;
	parser.footer = FOOTERTEXT;
	stage = STAGE_PREPARE;
	keepgoing = callOptionGroups();

	if (keepgoing) {
		stage = STAGE_PARSE;
	}

	return keepgoing;
}


void ArgState::run(int argc, const char** argv) {
	bool keepgoing = true;

	ukoct_ASSERT(stage == STAGE_PARSE, "Running must only happen at parsing stage.");


	// PARSING


	if (keepgoing) {
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
	}

	if (keepgoing) {
		keepgoing = callOptionGroups();
	}

	if (keepgoing) {
		// Input
		if (parser.firstArgs.size() > 1) {
			inputOperand.inputStream.fileName = *parser.firstArgs.back();
			dbg() << "Found input file \"" << inputOperand.inputStream.fileName << "\"." << std::endl;
		}

		for (auto& ezGroup : parser.groups) {
			const Option* option = NULL;

			for (auto& flag : ezGroup->flags) {
				auto it = options.find(*flag);
				if (it != options.end()) {
					option = it->second;
					break;
				}
			}

			ukoct_ASSERT(option != NULL, "Option Groups must register an option object.");

			std::vector<std::vector<std::string> > multiArgs;
			ezGroup->getMultiStrings(multiArgs);

			// If the option takes any arguments, or if giving an
			// argument is optional and was not given, ez will return
			// a non-empty multi-argument array[i][j] where each [i] is
			// the specific occurrence (that coincides in index with
			// the parseIndex vector in the OptionGroup), and [j] is
			// the split option (if any). array[i] should not be empty.
			if (!multiArgs.empty()) {
				for (size_t i = 0; i < multiArgs.size(); ++i) {
					Operand multiOperand;
					multiOperand.option = option;
					multiOperand.parseIndex = ezGroup->parseIndex.size() == multiArgs.size() ? ezGroup->parseIndex[i] : 0; // Because of the defaults
					multiOperand.ezGroup = ezGroup;
					multiOperand.args = multiArgs[i];
					multiOperand.stage = STAGE_PARSE;
					keepgoing = callOptionGroups(multiOperand);

					if (keepgoing) {
						operands.push_back(multiOperand);
					} else {
						break;
					}
				}

			} else {
				// In case the option doesn't take any arguments,
				// just insert each parseIndex to the operands list.
				// Those will be further sorted and then
				for (auto& parseIndex : ezGroup->parseIndex) {
					Operand operandOccurrence;
					operandOccurrence.option = option;
					operandOccurrence.parseIndex = parseIndex;
					operandOccurrence.ezGroup = ezGroup;
					operandOccurrence.stage = STAGE_PARSE;
					keepgoing = callOptionGroups(operandOccurrence);

					if (keepgoing) {
						operands.push_back(operandOccurrence);
					} else {
						break;
					}
				}
			}

			if (!keepgoing) {
				break;
			}
		}
	}

	if (keepgoing && totalActions == 0) {
		keepgoing = false;
		err() << "Please specify at least one action." << std::endl;
		err(false) << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		exitCode = RETEARG;
		keepgoing = false;
	}

	// Open files/streams

	if (keepgoing) {
		keepgoing = parseInputOperand(inputOperand) && parseOutputOperand(outputOperand);
	}

	if (keepgoing) {
		for (auto& operand : operands) {
			keepgoing = parseOperand(operand);
			if (!keepgoing) break;
		}
	}


	// INIT


	if (keepgoing) {
		stage = STAGE_INIT;
		std::sort(operands.begin(), operands.end());
		keepgoing = callOptionGroups();
	}

	if (keepgoing) {
		for (auto& operand : operands) {
			operand.stage = STAGE_INIT;
			keepgoing = callOptionGroups(operand);
			if (!keepgoing) {
				break;
			}
		}
	}


	// RUN


	if (keepgoing) {
		stage = STAGE_EXEC;
		keepgoing = callOptionGroups();
	}

	if (keepgoing) {
		unsigned int actionIdx = 1;

		for (auto& operand : operands) {
			operand.stage = STAGE_EXEC;
			operand.actionIdx = actionIdx;
			keepgoing = callOptionGroups(operand);

			if (!keepgoing) {
				break;
			} else {
				++actionIdx;
			}
		}
	}


	// PRINT RESULTS


	if (keepgoing && printTimings) {
		// TODO Print results
	}
}


void ArgState::finalize() throw() {
	stage = STAGE_FINISH;

	// Finalize operands
	for (auto& operand : operands) {
		operand.stage = STAGE_FINISH;

		for (int optionGroupId = OPTG_MIN_; optionGroupId <= OPTG_MAX_; ++optionGroupId) {
			const IOptionGroupCallback& gcb = *optionGroupCallbacks[optionGroupId];
			gcb(*this, &operand);
		}
	}

	// Close files/streams
	inputOperand.inputStream.close();
	inputOperand.outputStream.close();
	outputOperand.outputStream.close();
	outputOperand.outputStream.close();
	for (auto& operand : operands) {
		operand.inputStream.close();
		operand.outputStream.close();
	}

	// Finalize the state itself
	for (int optionGroupId = OPTG_MIN_; optionGroupId <= OPTG_MAX_; ++optionGroupId) {
		const IOptionGroupCallback& gcb = *optionGroupCallbacks[optionGroupId];
		gcb(*this);
	}

	operands.clear();
	variants.clear();
}


bool ArgState::registerOption(const Option& option) {
	bool registered = 0;

	ukoct_ASSERT(option.longFlag != NULL, "An option must provide at least a long flag.");

	if (option.longFlag != NULL) {
		auto it = options.find(option.longFlag);

		// Only include if it was not yet registered.
		if (it == options.end()) {
			ez::ezOptionValidator* validator = option.type ? new ez::ezOptionValidator(option.type) : NULL;

			if (option.shortFlag != NULL && option.longFlag != NULL) {
				parser.add(
					option.defaultVal
					, option.required
					, option.nargs
					, option.argsep
					, option.help
					, option.shortFlag
					, option.longFlag
					, validator
				);

			} else if (option.longFlag != NULL) {
				parser.add(
					option.defaultVal
					, option.required
					, option.nargs
					, option.argsep
					, option.help
					, option.longFlag
					, validator
				);
			}

			options[option.longFlag] = &option;
			registered = true;
		}
	}

	return registered;
}


void ArgState::printHelp(std::ostream& os) {
	// TODO Redo help to show contents from option groups
	std::string txt;
	parser.getUsage(txt);
	os << txt;
}


void ArgState::printVersion(std::ostream& os) {
	os << VERSIONTEXT << FOOTERTEXT << std::endl;
}


bool ArgState::parseOperand(Operand& operand) {
	return parseInputStream(operand) && parseOutputOperand(operand);
}


bool ArgState::parseInputOperand(Operand& operand) {
	return parseInputStream(operand);
}


bool ArgState::parseOutputOperand(Operand& operand) {
	return parseOutputStream(operand);
}


bool ArgState::parseInputStream(Operand& operand) {
	bool keepgoing = true;
	const Option& option = *operand.option;

	if (operand.inputStream.fileName.empty()) {
		operand.inputStream.stream = &nullstream;

	} else if (operand.inputStream.fileName.compare("-") == 0) {
		std::string buf;
		operand.inputStream.stringStream = new std::stringstream();
		operand.inputStream.stream = operand.inputStream.stringStream;

		while (std::getline(std::cin, buf)) {
			*(operand.inputStream.stringStream) << buf << std::endl;
		}

	} else {
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
	const Option& option = *operand.option;

	if (operand.outputStream.fileName.empty()) {
		operand.outputStream.stream = &nullstream;

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


bool ArgState::callOptionGroups() {
	bool keepgoing = true;

	for (int optionGroupId = OPTG_MIN_; optionGroupId <= OPTG_MAX_ && keepgoing; ++optionGroupId) {
		const IOptionGroupCallback& gcb = *optionGroupCallbacks[optionGroupId];
		ukoct_ASSERT(optionGroupCallbacks[optionGroupId] != NULL, "NULL Option group callback, recheck declarations.");
		keepgoing = gcb(*this);
	}

	return keepgoing;
}


bool ArgState::callOptionGroups(Operand& operand) {
	bool keepgoing = true;

	// Option 1: Call all optionGroups on all operands, regardless of its
	// original option and option group
#if 0
	for (int optionGroupId = OPTG_MIN_; optionGroupId <= OPTG_MAX_ && keepgoing; ++optionGroupId) {
		const IOptionGroupCallback& gcb = *optionGroupCallbacks[optionGroupId];

		switch (operand.stage) {
		case STAGE_INIT:
			operand.initTiming.stage(STAGE_INIT).stageName("init");
			operand.initTiming.start();
			break;
		case STAGE_RUN:
			operand.execTiming.stage(STAGE_EXEC).stageName("run");
			operand.execTiming.start();
			break;
		}

		keepgoing = gcb(*this, &operand);

		// Timing end
		switch(operand.stage) {
		case STAGE_INIT:
			operand.initTiming.end();
			operand.initTiming.round();
			break;
		case STAGE_RUN:
			operand.execTiming.end();
			operand.execTiming.round();
			break;
		}
	}
#endif
	// Option 2: Call only the option group declared on the option that the
	// operand belongs to.
	ukoct_ASSERT(operand.option != NULL, "Operand must have an option object.");
	const IOptionGroupCallback& gcb = *optionGroupCallbacks[operand.option->group];
	ukoct_ASSERT(gcb != NULL, "Invalid option declared for option, `gcb` cannot be NULL.");

	// Timing start
	switch (operand.stage) {
	case STAGE_INIT:
		operand.initTiming.stage(STAGE_INIT).stageName("init");
		operand.initTiming.start();
		break;
	case STAGE_RUN:
		operand.execTiming.stage(STAGE_EXEC).stageName("run");
		operand.execTiming.start();
		break;
	}

	// Run
	keepgoing = gcb(*this, &operand);

	// Timing end
	switch(operand.stage) {
	case STAGE_INIT:
		operand.initTiming.end();
		operand.initTiming.round();
		break;
	case STAGE_RUN:
		operand.execTiming.end();
		operand.execTiming.round();
		break;
	}

	return keepgoing;
}


bool ArgState::initOperand(Operand& operand) {
	return (!operand.inputStream.fileName.empty() ? initInputOperand(operand) : true) &&
			(!operand.outputStream.fileName.empty() ? initOutputOperand(operand) : true);
}


bool ArgState::initInputOperand(Operand& operand) {
	bool keepgoing = true;
	return keepgoing;
}


bool ArgState::initOutputOperand(Operand& operand) {
	bool keepgoing = true;
	return keepgoing;
}
