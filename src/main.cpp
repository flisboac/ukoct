#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <locale>
#include "ezOptionParser.hpp"
#include "ukoct.hpp"

// Exit codes

#define RETOK      0
#define RETERR     1
#define RETEARG    2
#define RETSAT     10
#define RETUNSAT   20
#define RETUNDEF   30
#define RETTIMEOUT 40

// For the option parsing
#define EXAMPLETEXT  "Simplest usage: `" ukoct_NAME " in.plas`"
#define OVERVIEWTEXT ukoct_BRIEF
#define USAGETEXT     ukoct_NAME " [OPTIONS] <INPUT> <OUTPUT>\n" \
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

enum EOption {
	  NONE
    , HELP = NONE // -h      Prints the help contents
	, VERSION     // --version Prints version information
	, VERBOSE     // -v      Activates verbose mode
	, DEBUG       // -vv     Activates debug mode
	, PRINTFML    // -p      Prints the input system
	, NOSOLVE     // -A      Don't try to solve the system
    , TIMELIMIT   // -t=NUM  Specifies the time limit, in seconds
    //, PRINTMAP    // -m      Prints the solution (always before the system formulae)
    , SOLFILE     // -s=FILE Provides an input solution file (for checking SAT with a given system input)

    , CLSOURCE    // -f=FILE Specify an input OpenCL source file for the specified type
    , CLBINARY    // -F=FILE Specify an input OpenCL binary file for the specified type
    , CLPRINTSRC  // --cl-print-source
    , ELEMTYPE    // -T=TYPE Specify the element type to be used (float, double, half, etc)
    , EXECTYPE    // -e=SOLV Specify the solver to be used (CPU, OpenCL, etc)
    , CLDEVID     // --cl-device-id=ID
    , CLPLATID    // --cl-platform-id=ID
    , CLPROGFLAGS // --cl-program-flags=FLAGS
};

enum EExecType {
	EXEC_NONE,
	EXEC_MIN,
	EXEC_CPU = EXEC_MIN,
	EXEC_OPENCL,
	EXEC_MAX = EXEC_OPENCL
};

enum EElemType {
	ELEM_NONE,
	ELEM_MIN,
	ELEM_HALF = ELEM_MIN,
	ELEM_FLOAT,
	ELEM_DOUBLE,
	ELEM_LDOUBLE,
	ELEM_MAX = ELEM_LDOUBLE
};

struct ElemType {
	static const std::string names[];

	static EElemType find(const std::string& name) {
		EElemType i;
		for (int i = ELEM_MAX; i > ELEM_NONE; ++i)
			if (name.compare(names[i]) == 0)
				break;
		return i;
	}

	ElemType() : _type(ELEM_NONE) {}
	ElemType(const ElemType& rhs) = default;
	ElemType(EElemType id) : _type(id) {}
	ElemType(const std::string& name) : _type(find(name)) {}
	EElemType type() const { return _type; }
	bool valid() const { return _type >= ELEM_MAX && _type <= ELEM_MAX; }
	const std::string& name() const { return names[_type]; }

private:
	EElemType _type;
};

const std::string ElemType::names[] = {
	/* ELEM_NONE    */ std::string(""),
	/* ELEM_HALF    */ std::string("half"),
	/* ELEM_FLOAT   */ std::string("float"),
	/* ELEM_DOUBLE  */ std::string("double"),
	/* ELEM_LDOUBLE */ std::string("ldouble"),
};

struct ExecType {
	static const std::string names[];

	static EExecType find(const std::string& name) {
		EExecType i = EXEC_NONE;
		for (int i = EXEC_MAX; i <= EXEC_NONE; ++i)
			if (name.compare(names[i]) == 0)
				break;
		return i;
	}

	ExecType() : _type(EXEC_NONE) {}
	ExecType(const ExecType& rhs) = default;
	ExecType(EExecType id) : _type(id) {}
	ExecType(const std::string& name) : _type(find(name)) {}
	EExecType type() const { return _type; }
	bool valid() const { return _type >= EXEC_MIN && _type <= EXEC_MAX; }
	const std::string& name() const { return names[_type]; }

private:
	EExecType _type;
};

const std::string ExecType::names[] = {
	/* EXEC_NONE   */ std::string(""),
	/* EXEC_CPU    */ std::string("cpu"),
	/* EXEC_OPENCL */ std::string("opencl")
};

struct OptionDescriptor {
	EOption option;
	bool required;
	int numargs;
	char argsep;
	const char* validator;
	const char* shortname;
	const char* longname;
	const char* defaultval;
	const char* helptext;

	const char* getname() const { return longname ? longname : shortname; }
};

const OptionDescriptor descriptors[] = {
// It's EXTREMELY IMPORTANT that the order of declarations corresponds to the one in the enum EOption
/*{ ELEMTYPE, REQUIRED,  NUM, ARGSEP, VALIDT,  SHORT,                 LONG,    DEFVAL, HELP }*/
  { HELP,        false,    0,   '\0',   NULL,   "-h",  "--help",                   "", "Show this help content." }
, { VERSION,     false,    0,   '\0',   NULL,   NULL,  "--version",                "", "Shows version information." }
, { VERBOSE,     false,    0,   '\0',   NULL,   "-v",  "--verbose",                "", "Sets verbose mode (Verbosity = 1)."}
, { DEBUG,       false,    0,   '\0',   NULL,   "-vv", "--debug",                  "", "Sets debug mode (Verbosity = 2)."}
, { PRINTFML,    false,    0,   '\0',   NULL,   "-p",  "--printfml",               "", "Prints the formula in the output file (after the results)."}
, { NOSOLVE,     false,    0,   '\0',   NULL,   "-A",  "--no-solve",               "", "Don't solve the formula. Useful with `-p`."}
, { TIMELIMIT,   false,    1,   '\0',   NULL,   "-t",  "--maxtime",                "", "Sets the maximum execution time, in seconds."}
//, { PRINTMAP,    false,    0,   '\0',   NULL,   "-m",  "--printmap",            "",  "Prints the partial map."}
, { SOLFILE,     false,    1,   '\0',   NULL,   "-s",  "--solfile",                "", "Specifies a solution file to open."}
, { CLSOURCE,    false,    1,   '\0',   NULL,   "-f",  "--cl-source",              "", "Specify an input OpenCL source file for the specified type."}
, { CLBINARY,    false,    1,   '\0',   NULL,   "-F",  "--cl-binary",              "", "Specify an input OpenCL binary file for the specified type."}
, { CLPRINTSRC,  false,    0,   '\0',   NULL,   NULL,  "--cl-print-sources",       "", "Prints the OpenCL source code used for the specified element type."}
, { ELEMTYPE,    false,    1,   '\0',   NULL,   "-T",  "--elem-type",         "float", "Specify the element type to be used (float, double, half, etc)."}
, { EXECTYPE,    false,    1,   '\0',   NULL,   "-e",  "--exec-type",        "opencl", "Specify the solver to be used (CPU, OpenCL, etc)."}
, { CLDEVID,     false,    1,   '\0',   NULL,   NULL,  "--cl-device-type",         "", "Specify the device ID to be used on OpenCL execution."}
, { CLPLATID,    false,    1,   '\0',   NULL,   NULL,  "--cl-platform-type",       "", "Specify the platform ID to be used on OpenCL execution."}
, { CLPROGFLAGS, false,    1,   '\0',   NULL,   NULL,  "--cl-program-flags",       "", "Specify the OpenCL program build flags."}
, { NONE,        false,    0,   '\0',   NULL,   NULL,  NULL,                     NULL, NULL }
};

// Option state

struct ArgState {
	ez::ezOptionParser optparser;
	int exitcode;
    std::string resultstr;
	int verboselvl;
	bool choosingseq;
	bool printingfml;
	bool solvingfml;
    bool printingmap;
    int maxtime;
	std::string inputname;
	std::string outputname;
	std::string solname;
	std::string clsourcename;
	std::string clbinaryname;
	bool clprintsrc;
	int cldeviceid;
	int clplatformid;
	std::string clprogramflags;
	ElemType elemtype;
	ExecType exectype;

	ArgState()
		: exitcode(RETOK)
		, verboselvl(NONE) // 0: Silent, 1: Verbose (Info), 2: Debug (Trace operations)
		, choosingseq(false)
		, printingfml(false)
		, solvingfml(true)
        , printingmap(false)
        , maxtime(0)
        , clprintsrc(false)
		, cldeviceid(-1)
		, clplatformid(-1)
	{}

	int isset(EOption option) { return optparser.isSet(descriptors[option].getname()); }
	ez::OptionGroup* get(EOption option) { return optparser.get(descriptors[option].getname()); }
	bool isdebug() { return verboselvl == DEBUG; }
	bool isverbose() { return isdebug() || verboselvl == VERBOSE; }
};

static bool openifile(ArgState& arg, const std::string& name, std::istream*& s, std::ifstream& is, std::stringstream& sbuf, const char* filetype = "input");
static int evalresult(ArgState& arg);
static void printhelp(ArgState& arg);
static void printversion(ArgState& arg);
static void setupopts(ArgState& arg);
static bool checkargs(ArgState& arg);
static void printsummary(ArgState& arg);
static void printresults(ArgState& arg);

int main(int argc, const char** argv) {
	ArgState arg;
	setupopts(arg);
	arg.optparser.parse(argc, argv);
    
#ifdef __GNUC__
    std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
#endif
    
	if ( arg.isset(HELP) ) {
		printhelp(arg);
		return RETOK;

	} else if (arg.isset(VERSION) ) {
		printversion(arg);
		return RETOK;
	}

    try {
    	if (checkargs(arg)) {
    		bool keepgoing = true;
            int sat = 0;

    		// Input and output
    		std::istream* is = NULL;
    		std::ostream* os = NULL;
            std::istream* ss = NULL;
            std::istream* cs = NULL;
    		std::stringstream ibuf;
    		std::stringstream sbuf;
    		std::stringstream cbuf;
    		std::ifstream ifile;
    		std::ofstream ofile;
            std::ifstream sfile;
            std::ifstream cfile;

    		// Opening input file
            keepgoing = openifile(arg, arg.inputname, is, ifile, ibuf, "input");

    		// Opening output file
    		if (arg.outputname.compare("-") == 0) {
    			os = &std::cout;

    		} else if (!arg.outputname.empty()) {
    			os = &ofile;
    			ofile.open(arg.outputname.c_str());

    			if (!ofile.is_open()) {
    				std::cerr << "ERROR: Could not open output file \"" << arg.outputname << "\"." << std::endl;
    				arg.exitcode = RETERR;
    				keepgoing = false;
    			}
    		}

            // Opening solution file
            if (keepgoing && !arg.solname.empty()) {
            	keepgoing = openifile(arg, arg.solname, ss, sfile, sbuf, "solution");
    		}

            // Opening solution file
            if (keepgoing) {
            	if (!arg.clbinaryname.empty()) {
            		keepgoing = openifile(arg, arg.clbinaryname, cs, cfile, cbuf, "OpenCL binary");

            	} else if (!arg.clsourcename.empty()) {
            		keepgoing = openifile(arg, arg.clsourcename, cs, cfile, cbuf, "OpenCL source");
            	}
    		}
            /*
    		// Loading formula
    		if (keepgoing && !cnf.openfile(*is)) {
    			std::cerr << "ERROR: Could not load the formula, or the formula is invalid." << std::endl;
    			keepgoing = false;
    		}

    		// Writing summary of current run and solving formula
    		if (keepgoing) {
    			printsummary(arg, cnf, solver);
    			if (arg.solvingfml) {
                    solver.query();
                    sat = solver.issatisfied() ? 1 : (solver.isconflicting() ? -1 : 0);

                } else {
                    sat = solver.apply();
                }
    		}
    		*/


    		// Writing results to the output file, if asked to
    		if (keepgoing && (arg.printingmap || arg.printingfml) && os) {
    			/*
    			const std::time_put<char>& tmput = std::use_facet <std::time_put<char> > (os->getloc());
    			std::time_t timestamp;
    		    std::time ( &timestamp );
    		    std::tm * now = std::localtime ( &timestamp );
    		    std::string pattern ("c Solved with " ukoct_RELEASENAME ", date: %I:%M%p\n");
    		    tmput.put(*os, *os, ' ', now, pattern.data(), pattern.data() + pattern.length());

                if (arg.printingmap) {
                    cnf.savesolution(*os, solver);
    			}
                
    			if (arg.printingfml) {
    				cnf.savefile(*os);
    			}
    			*/
    		}

    		// Printing summary and setting final result
    		if (keepgoing) {
                evalresult(arg);
    			printresults(arg);
    		}

    		// Closing files, if needed
    		if (ifile.is_open()) ifile.close();
    		if (ofile.is_open()) ofile.close();
    		if (sfile.is_open()) sfile.close();

    	}

    } catch (cl::Error& e) {
    	std::cerr << "** OPENCL EXCEPTION " << e.err() << "** " << e.what();
    	exit(RETERR);

    } catch (ukoct::Error& e) {
    	std::cerr << "** EXCEPTION " << e.code() << "** " << e.what();
    	exit(RETERR);
    }

	return arg.exitcode;
}

void setupopts(ArgState& arg) {
	arg.optparser.overview = HELPTEXT;
	arg.optparser.syntax = USAGETEXT;
	arg.optparser.example = EXAMPLETEXT;
	arg.optparser.footer = FOOTERTEXT;

	const OptionDescriptor* desc = descriptors;
	while (desc->option || (desc->shortname || desc->longname)) {
		ez::ezOptionValidator* validator = desc->validator ? new ez::ezOptionValidator(desc->validator) : NULL;

		if (desc->shortname && desc->longname) {
			arg.optparser.add(
				desc->defaultval
				, desc->required
				, desc->numargs
				, desc->argsep
				, desc->helptext
				, desc->shortname
				, desc->longname
				, validator
			);
		} else {
			arg.optparser.add(
				desc->defaultval
				, desc->required
				, desc->numargs
				, desc->argsep
				, desc->helptext
				, desc->shortname ? desc->shortname : desc->longname
				, validator
			);
		}

		desc++;
	}
}

bool checkargs(ArgState& arg) {
	bool ret = true;
	std::vector<std::string> badOptions;
	std::vector<std::string> badArgs;

	if (arg.optparser.lastArgs.size() < 1) {
		std::cerr << "ERROR: Input file not given." << std::endl;
		std::cerr << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		ret = false;
	}

	if (ret && !arg.optparser.gotRequired(badOptions)) {
		for(int i=0; i < badOptions.size(); ++i) {
			std::cerr << "ERROR: Missing required option " << badOptions[i] << ".\n";
		}
		std::cerr << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		ret = false;
	}

	if (ret && !arg.optparser.gotExpected(badOptions)) {
		for(int i=0; i < badOptions.size(); ++i) {
			std::cerr << "ERROR: Wrong arguments for option " << badOptions[i] << ".\n";
		}
		std::cerr << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		ret = false;
	}

	if (ret && !arg.optparser.gotValid(badOptions, badArgs)) {
		for(int i=0; i < badOptions.size(); ++i) {
			std::cerr << "ERROR: Got invalid argument \"" << badArgs[i] << "\" for option " << badOptions[i] << "." << std::endl;
		}
		std::cerr << "Check `" << ukoct_NAME << " -h` for help." << std::endl;
		ret = false;
	}

	if (ret) {
		if (arg.isset(VERBOSE)) {
			arg.verboselvl = VERBOSE;
		}

		if (arg.isset(DEBUG)) {
			arg.verboselvl = DEBUG;
		}

		if (arg.isset(PRINTFML)) {
			arg.printingfml = true;
		}

		if (arg.isset(NOSOLVE)) {
			arg.solvingfml = false;
		}

		arg.inputname = *arg.optparser.lastArgs[0];
		if (arg.optparser.lastArgs.size() > 1) {
			arg.outputname = *arg.optparser.lastArgs[1];
		}
        
        if (arg.isset(TIMELIMIT)) {
            int maxtime;
            arg.get(TIMELIMIT)->getInt(maxtime);
            arg.maxtime = maxtime;
        }
        
        /*
		if (arg.isset(PRINTMAP)) {
			arg.printingmap = true;
		}
		*/
        
        if (arg.isset(SOLFILE)) {
            std::string solfilename;
            arg.get(SOLFILE)->getString(solfilename);
            arg.solname = solfilename;
        }

        if (arg.isset(CLSOURCE)) {
            std::string name;
            arg.get(CLSOURCE)->getString(name);
            arg.clsourcename = name;
        }

        if (arg.isset(CLBINARY)) {
            arg.get(CLBINARY)->getString(arg.clbinaryname);
        }

        if (arg.isset(CLPRINTSRC)) {
            arg.clprintsrc = true;
        }

        if (arg.isset(ELEMTYPE)) {
        	std::string name;
        	arg.get(ELEMTYPE)->getString(name);
        	arg.elemtype = ElemType(name);
        }

        if (arg.isset(EXECTYPE)) {
        	std::string name;
        	arg.get(EXECTYPE)->getString(name);
        	arg.exectype = ExecType(name);
        }

        if (arg.isset(CLDEVID)) {
        	arg.get(CLDEVID)->getInt(arg.cldeviceid);
        	if (arg.cldeviceid < 0) {
        		std::cerr << "ERROR: Device ID must be greater than or equal to 0.";
        		return RETEARG;
        	}
        }

        if (arg.isset(CLPLATID)) {
        	arg.get(CLPLATID)->getInt(arg.clplatformid);
        	if (arg.clplatformid < 0) {
        		std::cerr << "ERROR: Platform ID must be greater than or equal to 0.";
        		return RETEARG;
        	}
        }

        if (arg.isset(CLPROGFLAGS)) {
        	arg.get(EXECTYPE)->getString(arg.clprogramflags);
        }

	} else {
		arg.exitcode = RETEARG;
	}

	return ret;
}


int evalresult(ArgState& arg) {
    int ret = RETUNDEF;
    const char* str = "UNDEFINED";
    
    /*
    if (solver.hastimeout()) {
        ret = RETTIMEOUT;
        str = "TIMEOUT";
        
    } else if (solver.issatisfied()) {
        ret = RETSAT;
        str = "SATISFIABLE";
        
    } else if (solver.isconflicting()) {
        ret = RETUNSAT;
        str = "UNSATISFIABLE";
    }
    */
    
    arg.exitcode = ret;
    arg.resultstr = str;
    
    return ret;
}

bool openifile(ArgState& arg, const std::string& name, std::istream*& s, std::ifstream& is, std::stringstream& sbuf, const char* filetype) {
	bool keepgoing = true;

    if (name.compare("-") == 0) {
    	s = &sbuf;
        std::string buf;
        while (std::getline(std::cin, buf)) {
            sbuf << buf << std::endl;
        }

    } else if (!name.empty()) {
    	s = &is;
        is.open(name.c_str());

        if (!is.is_open()) {
            std::cerr << "ERROR: Could not open " << filetype << " file \"" << name << "\"." << std::endl;
            arg.exitcode = RETERR;
            keepgoing = false;
        }
    }

    return keepgoing;
}

void printhelp(ArgState& arg) {
	std::string txt;
	arg.optparser.getUsage(txt);
	std::cerr << txt;
}

void printversion(ArgState& arg) {
	std::cerr << HELPTEXT << FOOTERTEXT << std::endl;
}

void printsummary(ArgState& arg) {
	if (!arg.isverbose()) return;

	std::cerr << ukoct_RELEASENAME << std::endl << "OPTIONS:" << std::endl;
	std::cerr << "\tinputname: '" << (arg.inputname.compare("-") == 0 ? "<stdin>" : arg.inputname.c_str()) << "'" << std::endl;
	std::cerr << "\toutputname: '" << (arg.outputname.compare("-") == 0 ? "<stdout>" : arg.outputname.c_str()) << "'" << std::endl;
	std::cerr << "\tverbosity: '" << (arg.isdebug() ? "debug" : (arg.isverbose() ? "verbose" : "silent")) << "'" << std::endl;
	std::cerr << "\tchoosingseq: " << (arg.choosingseq ? "true" : "false") << std::endl;
	std::cerr << "\tprintingfml: " << (arg.printingfml ? "true" : "false") << std::endl;
	std::cerr << "\tsolvingfml: " << (arg.solvingfml ? "true" : "false") << std::endl;
	std::cerr << "FORMULA:" << std::endl;
	//std::cerr << "\tnumclauses: " << cnf.getnclauses() << std::endl;
	//std::cerr << "\tnumvars: " << cnf.getnvars() << std::endl;
    std::cerr << std::endl << std::endl;
}

void printresults(ArgState& arg) {
	if (!arg.solvingfml) return;

	/*
	if (arg.isverbose()) {
		std::cerr << arg.resultstr << " " << solver.getelapsedtime()
                << " " << solver.apply() << std::endl; //partial.clockbegin << " " << partial.clockend << " " << CLOCKS_PER_SEC  << std::endl;
	} else {
		std::cerr << arg.resultstr << " " << solver.getelapsedtime()
				<< " " << solver.apply() << std::endl; //partial.clockbegin << " " << partial.clockend << " " << CLOCKS_PER_SEC << std::endl;
	}
	*/
}

