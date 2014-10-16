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


enum EStage {
	STAGE_PREPARE, // Initial stages of object construction.
	STAGE_PARSE,   // Command-line parsing and gathering of basic (general) options.
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


struct ArgState;
struct Operand;
struct InputStream;
struct OutputStream;
struct Timing;

struct OptionGroup;
struct Option;
struct TypeName;
struct OperDetailValue;

struct IOptionCallback;
struct IOperationCallback;


struct IOptionCallback {
	virtual ~IOptionCallback() {}
	virtual bool operator()(ArgState& A, Operand& operand) const = 0;
};


struct NullStream : public std::stringstream {
	NullStream() { setstate(std::ios_base::badbit); }
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


struct Timing {
	clock_t clockStart;
	clock_t clockEnd;
	Timing() : clockStart(0), clockEnd(0) {}
	Timing(const Timing& other) = default;
	inline void start() { clockStart = clock(); }
	inline void end() {
		ukoct_ASSERT(isStarted(), "end() can only be called after start().");
		clockEnd = clock();
	}
	inline bool isStarted() const { return clockStart != 0; }
	inline bool isFinished() const { return clockEnd != 0; }
	inline clock_t diff() const {
		ukoct_ASSERT(isFinished(), "diff() can only be called after end().");
		return clockEnd - clockStart;
	}
	inline clock_t rem() const {
		return diff() % CLOCKS_PER_SEC;
	}
	inline void print(std::ostream& os, int precision = 5) const {
		double tsec = sec();
		const char* suffix = "s";
		double t = sec();

		if (t < 1) {
			t *= 1000;
			suffix = "ms";

			if (t < 1) {
				t *= 1000;
				suffix = "us";
			}

		} else if (t >= 1000) {
			t /= 60;
			suffix = "m";

			if (t >= 1000) {
				t /= 24;
				suffix = "h";
			}
		}

		os << std::fixed << std::setw(0) << std::setprecision(precision) << t << suffix;
	}
	inline double sec() const {
		ukoct_ASSERT(isStarted(), "end() can only be called after start().");
		return static_cast<double>(diff()) / static_cast<double>(CLOCKS_PER_SEC);
	}
	inline double msec() const {
		return sec() * 1000;
	}
	inline double usec() const {
		return sec() * 1000 * 1000;
	}
	inline double min() const {
		return sec() / 60;
	}
	inline double hour() const {
		return sec() / 24;
	}
};


inline std::ostream& operator<<(std::ostream& os, const Timing& timing) {
	timing.print(os);
	return os;
}


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
		if (fileStream != NULL) delete fileStream;
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
	EStage stage;

	// Performance info
	Timing initTiming;
	Timing execTiming;

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
		: optionId(other.optionId)
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
		: optionId(0)
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


static NullStream nullstream;

struct ArgState {
	ez::ezOptionParser parser;
	int stage;
	int exitCode;
	std::ostream* slog;
	std::ostream* serr;
	std::ostream* sout;
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
	bool parseArgs(int argc, const char** argv);
	bool parseOperands();
	void executeOperands();

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



template <typename T> plas::Problem<T, plas::DenseMatrix>* loadProblem(ArgState& A, Operand& O) {
	plas::Problem<T, plas::DenseMatrix>* problem = new plas::Problem<T, plas::DenseMatrix>();
	bool ret = plas::open(*O.inputStream.stream, *problem, &A.log(false));

	if (ret) {
		if (problem->problemType() == plas::PROBLEM_OCT) {
			plas::Problem<T, plas::DenseMatrix>* other = new plas::Problem<T, plas::DenseMatrix>();
			ret = plas::toOctDiffProblem(*problem, *other, &A.log(false));
			delete problem;
			problem = other;
			ret = false;

		} else if (problem->problemType() != plas::PROBLEM_OCTDIFF) {
			A.err() << "Invalid problem type on input '" << O.inputStream.fileName << "'." << std::endl;
			delete problem;
			problem = NULL;
			ret = false;
		}
	}

	if (ret) {
		if (O.optionId == OPT_OP_PUSHC) {
			A.err() << "pushc not yet implemented, sorry!" << std::endl;
			A.exitCode = RETNOTIMPL;

		} else if (O.optionId == OPT_OP_PUSHD) {
			A.err() << "pushd not yet implemented, sorry!" << std::endl;
			A.exitCode = RETNOTIMPL;
		}
	}

	return problem;
}


template <typename T> void finalizeProblem(ArgState& A, Operand& O) {
	if (O.octCons != NULL) {
		plas::OctConstraint<T>* c = reinterpret_cast<plas::OctConstraint<T>*>(O.octCons);
		delete c;
		O.octCons = NULL;
	}

	if (O.diffCons != NULL) {
		plas::OctDiffConstraint<T>* c = reinterpret_cast<plas::OctDiffConstraint<T>*>(O.diffCons);
		delete c;
		O.diffCons = NULL;
	}
}



#endif /* MAIN_HPP_ */
