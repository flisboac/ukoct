#ifndef MAIN_HPP_
#define MAIN_HPP_


#include <cstddef>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "ezOptionParser.hpp"
#include "ukoct.hpp"


#define RETOK      0
#define RETERR     1
#define RETEARG    2
#define RETNOTIMPL 3
#define RETSAT     RETOK
#define RETUNSAT   20
#define RETUNDEF   30
#define RETTIMEOUT 40

#define ukoct_AVAILABLE_IMPL "cpu, opencl"

#define OVERVIEWTEXT ukoct_BRIEF

#define USAGETEXT \
	ukoct_NAME " [OPTIONS] <INPUT> [OPER [INPUT]] [-o OUTPUT]\n\n" \
	"If <INPUT> equals \"-\", STDIN is used.\n" \
	"If <OUTPUT> equals \"-\", STDOUT is used."

#define EXAMPLETEXT  "\t$ " ukoct_NAME " in.plas +sat?\n"

#define HEADERTEXT \
	ukoct_RELEASENAME "\n" \
	ukoct_BRIEF

#define DISCLAIMER "\n" \
	"This is a free and open-source software under MIT license.\n" \
	"Please refer to the source code for full license and conditions.\n" \
	"There is NO warranty, not even for MERCHANTABILITY or FITNESS FOR A\n"\
	"PARTICULAR PURPOSE.\n"

#define VERSIONTEXT \
	ukoct_RELEASENAME "\n" \
	ukoct_BRIEF "\n" \
	"Authors: " ukoct_AUTHORS "\n" \
	DISCLAIMER

#define HELPTEXT \
	HEADERTEXT

#define FOOTERTEXT "\n" \
	"For bug reports, feature requests and other issues,\n" \
	"please use the issue tracker for the project at GitHub:\n" \
	"<https://github.com/flisboac/ukoct/issues>\n"


#ifdef ukoct_DEBUG
#	define ukoct_DBG(o) o
#else
#	define ukoct_DBG(o)
#endif


//
// OPTION LISTINGS
//


enum EHelpContents {
	HELP_VERSION,
	HELP_USAGE,
	HELP_SECTION,
	HELP_EXAMPLES,
	HELP_FOOTER
};


enum EStage {
	STAGE_PREPARE, // Initial stages of object construction, registering of options on the option parser, etc
	STAGE_PARSE,   // Command-line parsing and validation
	STAGE_INIT,    // Instantiation of implementation states, matrices and operators.
	STAGE_EXEC,    // Operations' execution.
	STAGE_FINISH   // Destruction of used objects (implementation states and all the like)
};


enum EOptionGroup {
	OPTG_NONE
	, OPTG_GENERAL
	, OPTG_DETAILS
	, OPTG_OPER

	, OPTG_MIN_ = OPTG_GENERAL
	, OPTG_MAX_ = OPTG_OPER
	, OPTG_LAST = OPTG_OPER
};


enum EVerboseLevel {
	VERB_SILENT = -1,
	VERB_NORMAL,
	VERB_VERBOSE,
	VERB_DEBUG
};


//
// TYPEDEFS
//


struct IOptionGroupCallback;
struct ArgState;
struct Operand;
struct InputStream;
struct OutputStream;

struct Option;
struct TypeName;
struct VariantDefinition;

struct NullStream;


//
// STRUCTS
//


struct NullStream : public std::stringstream {
	NullStream() { setstate(std::ios_base::badbit); }
};


struct IOptionGroupCallback {
	virtual ~IOptionGroupCallback() {}
	virtual EOptionGroup groupId() const = 0;
	virtual bool operator()(ArgState& A, Operand* operand = NULL) const = 0;
	virtual void help(std::ostream& os, EHelpContents which) const = 0;
};


struct Option {
	int id;
	EOptionGroup group;
	ukoct::EOperation operation;
	bool required;
	int nargs;
	char argsep;
	const char* defaultVal;
	const char* type;
	const char* shortFlag;
	const char* longFlag;
	const char* help;
};


struct TypeName {
	int id;
	const char* name;
};


struct VariantDefinition {
	const char* name;
	ukoct::OperationDetails group;
	ukoct::OperationDetails details;
	const char* help;
};


struct InputStream {
	std::string fileName;
	std::istream* stream;
	std::ifstream* fileStream;       // NOTE: Streams cannot be declared by value if the class is supposed to be
	std::stringstream* stringStream; // used with a STL container, because streams are neither movable nor copyable.
	InputStream()
		: fileName()
		, stream(NULL)
		, fileStream(NULL)
		, stringStream(NULL)
		{}
	InputStream(const char* fn)
		: fileName(fn)
		, stream(NULL)
		, fileStream(NULL)
		, stringStream(NULL)
		{}
	InputStream(const InputStream& rhs) = default;
	~InputStream() {
		close();
		delete fileStream;
		delete stringStream;
	}
	operator std::istream&() { return *stream; }
	inline bool isParsed() { return stream != NULL; }
	inline void close() { if (fileStream != NULL && fileStream->is_open()) fileStream->close(); }
};


struct OutputStream {
	std::string fileName;
	std::ostream* stream;
	std::ofstream* fileStream;       // NOTE: Streams cannot be declared by value if the class is supposed to be
	OutputStream()
		: fileName()
		, stream(NULL)
		, fileStream(NULL)
		{}
	OutputStream(const char* fn)
		: fileName(fn)
		, stream(NULL)
		, fileStream(NULL)
		{}
	OutputStream(const OutputStream& rhs) = default;
	~OutputStream() {
		close();
		delete fileStream;
	}
	operator std::ostream&() { return *stream; }
	inline bool isParsed() { return stream != NULL; }
	inline void close() { if (fileStream != NULL && fileStream->is_open()) fileStream->close(); }
};


struct Operand {
	// General options
	const Option* option;
	int optionId;
	int groupId;
	unsigned int actionIdx;
	ez::OptionGroup* ezGroup;
	std::vector<std::string> args;
	int parseIndex;
	EStage stage;

	// Performance info
	ukoct::CpuTiming initTiming;
	ukoct::CpuTiming execTiming;

	// For operators
	InputStream inputStream;
	OutputStream outputStream;
	plas::var_t var;
	void* diffCons;
	void* octCons;
	void* state;

	Operand()
		: option(NULL)
		, optionId(0)
		, groupId(0)
		, actionIdx(0)
		, ezGroup(NULL)
		, args()
		, parseIndex(-1)
		, stage(STAGE_PREPARE)
		, initTiming()
		, execTiming()
		, inputStream()
		, outputStream()
		, var(0)
		, diffCons(NULL)
		, octCons(NULL)
		, state(NULL)
		{}
	Operand(const Operand& other)
		: option(other.option)
		, optionId(other.optionId)
		, groupId(other.groupId)
		, actionIdx(other.actionIdx)
		, ezGroup(other.ezGroup)
		, args(other.args)
		, parseIndex(other.parseIndex)
		, stage(STAGE_PREPARE)
		, initTiming()
		, execTiming()
		, inputStream(other.inputStream)
		, outputStream(other.outputStream)
		, var(other.var)
		, diffCons(other.diffCons)
		, octCons(other.octCons)
		, state(other.state)
		{}
	Operand(const char* inpf, const char* outf)
		: option(NULL)
		, optionId(0)
		, groupId(0)
		, actionIdx(0)
		, ezGroup(NULL)
		, args()
		, parseIndex(-1)
		, stage(STAGE_PREPARE)
		, initTiming()
		, execTiming()
		, inputStream(inpf)
		, outputStream(outf)
		, var(0)
		, diffCons(NULL)
		, octCons(NULL)
		, state(NULL)
		{}
	inline bool operator<(const Operand& other) {
		return compare(other) < 0;
	}
	inline int compare(const Operand& other) {
		int ret = 0;
		int thisOptionId = option != NULL ? option->id : optionId;
		int otherOptionId = other.option != NULL ? other.option->id : other.optionId;
		int thisGroupId = option != NULL ? option->group : groupId;
		int otherGroupId = other.option != NULL ? other.option->group : other.groupId;
		ret = (thisGroupId > otherGroupId) - (thisGroupId < otherGroupId);
		ret = ret != 0 ? ret : (thisOptionId > otherOptionId) - (thisOptionId < otherOptionId);
		return ret != 0 ? ret : (ezGroup->parseIndex > other.ezGroup->parseIndex) - (ezGroup->parseIndex < other.ezGroup->parseIndex);
	}
};

extern NullStream nullstream;

struct ArgState {
	ez::ezOptionParser parser;
	int stage;
	int exitCode;
	std::ostream* slog;
	std::ostream* serr;
	std::ostream* sout;
	EVerboseLevel verbose;
	int maxTime;
	size_t totalActions;
	Operand inputOperand;
	Operand outputOperand;
	ukoct::EImplementation implType;
	ukoct::EElemType elemType;
	std::map<ukoct::EOperation, ukoct::OperationDetails> variants;
	std::map<std::string, const Option*> options;
	std::vector<Operand> operands;
	ArgState()
		: exitCode(0)
		, stage(STAGE_PREPARE)
		, verbose(VERB_DEBUG)
		, maxTime(0)
		, totalActions(0)
		, inputOperand("-", "")
		, outputOperand("", "-")
		, implType(ukoct::IMPL_OPENCL)
		, elemType(ukoct::ELEM_FLOAT)
		, sout(&std::cout)
		, slog(&std::cerr)
		, serr(&std::cerr)
		{}
	~ArgState();
	inline std::ostream& dbg(bool withHeader = true)  { return verbose >= VERB_DEBUG ? (slog != NULL ? *slog << (withHeader ? ". [ DBG] " : "") : nullstream) : nullstream; }
	inline std::ostream& log(bool withHeader = true)  { return verbose >= VERB_VERBOSE ? (slog != NULL ? *slog << (withHeader ? "- [ LOG] " : "") : nullstream) : nullstream; }
	inline std::ostream& prof(bool withHeader = true) { return verbose >= VERB_NORMAL ? (slog != NULL ? *slog << (withHeader ? "@ [PROF] " : "") : nullstream) : nullstream; }
	inline std::ostream& warn(bool withHeader = true) { return verbose >= VERB_NORMAL ? (slog != NULL ? *slog << (withHeader ? "! [WARN] " : "") : nullstream) : nullstream; }
	inline std::ostream& err(bool withHeader = true)  { return verbose >= VERB_NORMAL ? (serr != NULL ? *serr << (withHeader ? "* [ ERR] " : "") : nullstream) : nullstream; }
	bool initialize();
	void run(int argc, const char** argv);
	void finalize() throw();
	bool registerOption(const Option& option);
	void printHelp(std::ostream& os);
	void printVersion(std::ostream& os);
	bool parseOperand(Operand& operand);
	bool parseInputOperand(Operand& operand);
	bool parseOutputOperand(Operand& operand);

private:
	bool parseInputStream(Operand& operand);
	bool parseOutputStream(Operand& operand);
};


//
// VARIABLES AND VALUES
//


extern const TypeName operNames[ukoct::OPER_MAX_ + 1];
extern const TypeName elemNames[ukoct::ELEM_MAX_ + 1];
extern const TypeName implNames[ukoct::IMPL_MAX_ + 1];

const size_t variantDefinitions_sz = 10;
extern const VariantDefinition variantDefinitions[variantDefinitions_sz];

extern const IOptionGroupCallback* optionGroupCallbacks[OPTG_MAX_ + 1];


//
// FUNCTIONS
//


template <typename T> const T* findByName(const char* str, const T* list, size_t size) {
	const T* ret = NULL;
	for (size_t i = 0; i < size && ret == NULL; ++i, ++list)
		if (list->name != NULL && strcmp(str, list->name))
			ret = list;
	return ret;
}


#endif /* MAIN_HPP_ */
