#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <cstddef>
#include <sstream>
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

#define EXAMPLETEXT  "Simplest usage: `" ukoct_NAME " in.plas` (default operator: closure)"
#define OVERVIEWTEXT ukoct_BRIEF
#define USAGETEXT     ukoct_NAME " [OPTIONS] <INPUT> [OPER [INPUT]] [-o OUTPUT]\n" \
	"If <INPUT> equals \"-\", STDIN is used.\n" \
	"If <OUTPUT> equals \"-\", STDOUT is used."

#define HEADERTEXT \
	VERSIONTEXT "\n" \
	OVERVIEWTEXT

#define FOOTERTEXT "\n\n" ukoct_NAME " is provided without warranty blabla (and I really need to put a proper license on this).\n"

#define VERSIONTEXT \
	ukoct_RELEASENAME

#define HELPTEXT \
	HEADERTEXT "\n" \
	"UK because of UKC. Go google it, you'll understand (maybe?).\n" \
	"Authors: " ukoct_AUTHORS


//
// OPTION LISTINGS
//


enum EStage {
	STAGE_PREPARE, // Initial stages of object construction.
	STAGE_PARSE,   // Command-line parsing and gathering of basic (general) options.
	STAGE_PREINIT, // Execute
	STAGE_INIT,    // Instantiation of implementation states, matrices and operators.
	STAGE_RUN,     // Operations' execution.
	STAGE_FINISH   // Destruction of used objects (implementation states and all the like)
};


enum EAction {
	ACT_NONE
	, ACT_HELP
	, ACT_VERSION
	, ACT_CLPRINTSRC
	, ACT_OPER

	, ACT_MIN_ = ACT_HELP
	, ACT_MAX_ = ACT_OPER
};


enum EOptionGroup {
	OPTG_NONE
	, OPTG_GENERAL
	, OPTG_DETAILS
	, OPTG_OP

	, OPTG_MIN_ = OPTG_GENERAL
	, OPTG_MAX_ = OPTG_OP
	, OPTG_LAST = OPTG_OP
};


enum EOption {
	OPT_NONE
	, OPT_INPUT = OPT_NONE

  	// General Options
    , OPT_HELP        // -h, --help Prints the help contents
	, OPT_VERSION     // --version Prints version information
	, OPT_SILENT      // -V      Activates silent mode
	, OPT_VERBOSE     // -v      Activates verbose mode
    , OPT_TIMELIMIT   // -t=NUM  Specifies the time limit, in seconds
    , OPT_OUTPUT      // -o=FILE Specifies the output
    , OPT_ELEMTYPE    // -e=TYPE Specify the element type to be used (float, double, half, etc)
    , OPT_EXECTYPE    // -x=SOLV Specify the solver to be used (CPU, OpenCL, etc)
    , OPT_OPERVARIANT // --variant=NAME Forces the solver to use a specific operator variant.

    // OpenCL options
    , OPT_CLSOURCE    // --cl-source=FILE Specify an input OpenCL source file for the specified type
    , OPT_CLBINARY    // --cl-binary=FILE Specify an input OpenCL binary file for the specified type
    , OPT_CLDEVID     // --cl-device-id=ID
    , OPT_CLPLATID    // --cl-platform-id=ID
    , OPT_CLPROGFLAGS // --cl-program-flags=FLAGS
    , OPT_CLPRINTSRC  // --cl-print-source=FILE

    // Unary Query operators
    , OPT_OP_ISSAT   // -sat?  consistent
    , OPT_OP_ISISAT  // -isat? intConsistent
    , OPT_OP_ISCOH   // -coh?  coherent
    , OPT_OP_ISCL    // -cl?   isClosed
    , OPT_OP_ISSCL   // -scl?  isStronglyClosed
    , OPT_OP_ISTCL   // -tcl?  isTightlyClosed
    , OPT_OP_ISWCL   // -wcl?  isWeaklyClosed
    , OPT_OP_ISTOP   // -top?  isTop
    , OPT_OP_CL    // -cl    closure
    , OPT_OP_TCL   // -tcl   tightClosure

    // Unary operators
    , OPT_OP_ECHO  // -echo  echo
    , OPT_OP_SP    // -sp    shortestPath
    , OPT_OP_STR   // -str   strengthen
    , OPT_OP_TIGHT // -tight tighen
    , OPT_OP_TOP   // -top   top

    // Binary operators
    , OPT_OP_PUSHD // -pushd push diff constraint
    , OPT_OP_PUSHC // -pushc push oct constraint
    , OPT_OP_POP   // -pop   Forget variable

    // Binary inter-matrix Operators
    , OPT_OP_EQ  // -eq   equal
    , OPT_OP_NEQ // -neq  notequal
    , OPT_OP_INC // -inc  includes
    , OPT_OP_AND // -and  union
    , OPT_OP_OR   // -or   intersection

    , OPT_MIN_ = OPT_HELP
    , OPT_MAX_ = OPT_OP_OR
};


struct OptionGroup {
	EOptionGroup id;
	const char* name;
	const char* help;
};


struct Option {
	EOption id;
	EOptionGroup group;
	EAction action;
	ukoct::EOperation operation;
	bool required;
	int nargs;
	char argsep;
	const char* defaultVal;
	const char* type; // Validator
	const char* shortFlag;
	const char* longFlag;
	const char* help;
};


struct TypeName {
	int id;
	const char* name;
};


struct OperDetailValue {
	const char* name;
	ukoct::OperationDetails group;
	ukoct::OperationDetails details;
	const char* help;
};

const size_t operDetailValues_sz = 10;
extern const OptionGroup optionGroups[OPTG_MAX_ + 1];
extern const Option options[OPT_MAX_ + 1];
extern const TypeName operNames[ukoct::OPER_MAX_ + 1];
extern const OperDetailValue operDetailValues[operDetailValues_sz];
extern const TypeName elemNames[ukoct::ELEM_MAX_ + 1];
extern const TypeName implNames[ukoct::IMPL_MAX_ + 1];


//
// PARSER STATE
//


enum EVerboseLevel {
	VERB_SILENT = -1,
	VERB_NORMAL,
	VERB_VERBOSE,
	VERB_DEBUG
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
		if (fileStream != NULL) delete fileStream;
		if (stringStream != NULL) delete stringStream;
	}
	inline bool isParsed() { return stream != NULL; }
};


struct OutputStream {
	std::string fileName;
	std::ostream* stream;
	std::ofstream* fileStream;       // NOTE: Streams cannot be declared by value if the class is supposed to be
	std::stringstream* stringStream; // used with a STL container, because streams are neither movable nor copyable.
	OutputStream()
		: fileName()
		, stream(NULL)
		, fileStream(NULL)
		, stringStream(NULL)
		{}
	OutputStream(const char* fn)
		: fileName(fn)
		, stream(NULL)
		, fileStream(NULL)
		, stringStream(NULL)
		{}
	OutputStream(const OutputStream& rhs) = default;
	~OutputStream() {
		if (fileStream != NULL) delete fileStream;
		if (stringStream != NULL) delete stringStream;
	}
	inline bool isParsed() { return stream != NULL; }
};


struct Operand {
	// General options
	// OPT_NONE denotes a positional argument
	int optionId;
	ez::OptionGroup* ezGroup;
	std::vector<std::string> args;
	int parseIndex;

	// For operators
	InputStream inputStream;
	OutputStream outputStream;
	plas::var_t var;
	void* diffCons;
	void* octCons;
	void* state;

	Operand()
		: optionId(0)
		, ezGroup(NULL)
		, args()
		, parseIndex(-1)
		, inputStream()
		, outputStream()
		, var(0)
		, diffCons(NULL)
		, octCons(NULL)
		, state(NULL)
		{}
	Operand(const Operand& other)
		: optionId(other.optionId)
		, ezGroup(other.ezGroup)
		, args(other.args)
		, parseIndex(other.parseIndex)
		, inputStream(other.inputStream)
		, outputStream(other.outputStream)
		, var(other.var)
		, diffCons(other.diffCons)
		, octCons(other.octCons)
		, state(other.state)
		{}
	Operand(const char* inpf, const char* outf)
		: optionId(0)
		, ezGroup(NULL)
		, args()
		, parseIndex(-1)
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
		int ret = (options[optionId].group > options[other.optionId].group) - (options[optionId].group < options[other.optionId].group);
		ret = ret != 0 ? ret : (options[optionId].action > options[other.optionId].action) - (options[optionId].action < options[other.optionId].action);
		return ret != 0 ? ret : (ezGroup->parseIndex > other.ezGroup->parseIndex) - (ezGroup->parseIndex < other.ezGroup->parseIndex);
	}
	inline ez::OptionGroup& argGroup() const { return *ezGroup; }
	inline const Option& option() const { return options[optionId]; }
};


struct OperDetails {
	std::string name;
	ukoct::OperationDetails details;
	void* oper;
	OperDetails() : oper(NULL) {}
	OperDetails(const OperDetails& rhs) = default;
};


struct NullStream : public std::stringstream {
	NullStream() { setstate(std::ios_base::badbit); }
};

static NullStream nullstream;

struct ArgState {
	ez::ezOptionParser parser;
	int stage;
	int exitCode;
	std::ostream* slog;
	std::ostream* serr;
	EVerboseLevel verbose;
	int maxTime;
	Operand inputOperand;
	Operand outputOperand;
	ukoct::EImplementation implType;
	ukoct::EElemType elemType;
	std::map<ukoct::EOperation, OperDetails> variants;
	std::vector<Operand> operands;
	ArgState()
		: exitCode(0)
		, stage(STAGE_PREPARE)
		, verbose(VERB_DEBUG)
		, maxTime(0)
		, inputOperand("-", "")
		, outputOperand("", "-")
		, implType(ukoct::IMPL_OPENCL)
		, elemType(ukoct::ELEM_FLOAT)
		, slog(&std::cerr)
		, serr(&std::cerr)
		{}
	inline std::ostream& dbg(bool withHeader = true)  { return verbose >= VERB_DEBUG ? (slog != NULL ? *slog << (withHeader ? ". [DBG] " : "") : nullstream) : nullstream; }
	inline std::ostream& log(bool withHeader = true)  { return verbose >= VERB_VERBOSE ? (slog != NULL ? *slog << (withHeader ? "- [LOG] " : "") : nullstream) : nullstream; }
	inline std::ostream& warn(bool withHeader = true) { return verbose >= VERB_NORMAL ? (slog != NULL ? *slog << (withHeader ? "! [WARN] " : "") : nullstream) : nullstream; }
	inline std::ostream& err(bool withHeader = true)  { return verbose >= VERB_NORMAL ? (serr != NULL ? *serr << (withHeader ? "* [ERROR] " : "") : nullstream) : nullstream; }
	bool initialize();
	bool parseArgs(int argc, const char** argv);
	bool parseOperands();
	void run();

private:
	bool parseInputOperand(Operand& operand);
	bool parseOutputOperand(Operand& operand);
	bool parseInputStream(Operand& operand);
	bool parseOutputStream(Operand& operand);
	void printHelp();
	void printVersion();
};


struct IOperationCallback {
	virtual ~IOperationCallback() {}
	virtual bool operator()(ArgState& A, Operand& operand) const = 0;
	virtual void source(std::string& str) const { }
};


extern const IOperationCallback* operationCallbacks[ukoct::IMPL_MAX_ + 1][ukoct::ELEM_MAX_ + 1];

extern const IOperationCallback* const ocb_cpu_flt;
extern const IOperationCallback* const ocb_cpu_dbl;
extern const IOperationCallback* const ocb_cpu_ldbl;
extern const IOperationCallback* const ocb_opencl_flt;


#endif /* MAIN_HPP_ */
