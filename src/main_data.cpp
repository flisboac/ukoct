
#include "main.hpp"

const OptionGroup optionGroups[OPTG_MAX_ + 1] = {
  { OPTG_NONE   , NULL               , NULL }
, { OPTG_GENERAL, "General Options"  , "Options that can be used in any operator or command." }
, { OPTG_DETAILS , "OpenCL Options"   , "Various options and flags for the OpenCL backend." }
, { OPTG_OP     , "Octagon Operators", "All Octagon domain's operators." }
};

const Option options[OPT_MAX_ + 1] = {
//  id              , group       , action        , oper                        , reqd , na, sep , defv    , vald, shrt, long                , help
  { OPT_INPUT       , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, NULL                , "The input file. Btw, this help content won't appear..." }

// General Options
, { OPT_HELP        , OPTG_GENERAL, ACT_HELP      , ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, "-h", "--help"            , "Displays help content." }
, { OPT_VERSION     , OPTG_GENERAL, ACT_VERSION   , ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, NULL, "--version"         , "Displays version information." }
, { OPT_SILENT      , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, "-V", "--silent"          , "Disables all messages." }
, { OPT_VERBOSE     , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, "-v", "--verbose"         , "Displays additional information. Use -vv for debug mode." }
, { OPT_TIMELIMIT   , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', "0"     , "u4", "-t", "--time-limit"      , "Sets up a time limit for operation execution. Given in milliseconds." }
, { OPT_OUTPUT      , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', "-"     , NULL, "-o", "--output-file"     , "Specifies an output file." }
, { OPT_ELEMTYPE    , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', "float" , NULL, "-e", "--elem-type"       , "Specifies the element type to be used. The available types are dependant on the implementation type. The default type is 'float'." }
, { OPT_EXECTYPE    , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', "opencl", NULL, "-x", "--exec-type"       , "Specifies the implementation to be used. The default implementation is 'opencl'. The available implementations are (" ukoct_AVAILABLE_IMPL ")" }
, { OPT_OPERVARIANT , OPTG_GENERAL, ACT_NONE      , ukoct::OPER_NONE            , false,  2,  ':', ""      , NULL, NULL, "--variant"         , "help" }

// OpenCL options
, { OPT_CLSOURCE    , OPTG_DETAILS , ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-source"       , "help" }
, { OPT_CLBINARY    , OPTG_DETAILS , ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-binary"       , "help" }
, { OPT_CLDEVID     , OPTG_DETAILS , ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', ""      , "u4", NULL, "--cl-device-id"    , "help" }
, { OPT_CLPLATID    , OPTG_DETAILS , ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', ""      , "u4", NULL, "--cl-platform-id"  , "help" }
, { OPT_CLPROGFLAGS , OPTG_DETAILS , ACT_NONE      , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "--cl-program-flags", "help" }
, { OPT_CLPRINTSRC  , OPTG_DETAILS , ACT_CLPRINTSRC, ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, NULL, "--cl-print-src"    , "help" }

// Unary Query operator
, { OPT_OP_ISSAT    , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISCONSISTENT    , false,  0, '\0', ""      , NULL, NULL, "+sat?"             , "help" }
, { OPT_OP_ISISAT   , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISINTCONSISTENT , false,  0, '\0', ""      , NULL, NULL, "+isat?"            , "help" }
, { OPT_OP_ISCOH    , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISCOHERENT      , false,  0, '\0', ""      , NULL, NULL, "+coh?"             , "help" }
, { OPT_OP_ISCL     , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISCLOSED        , false,  0, '\0', ""      , NULL, NULL, "+cl?"              , "help" }
, { OPT_OP_ISSCL    , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISSTRONGLYCLOSED, false,  0, '\0', ""      , NULL, NULL, "+scl?"             , "help" }
, { OPT_OP_ISTCL    , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISTIGHTLYCLOSED , false,  0, '\0', ""      , NULL, NULL, "+tcl?"             , "help" }
, { OPT_OP_ISWCL    , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISWEAKLYCLOSED  , false,  0, '\0', ""      , NULL, NULL, "+wcl?"             , "help" }
, { OPT_OP_ISTOP    , OPTG_OP     , ACT_OPER      , ukoct::OPER_ISTOP           , false,  0, '\0', ""      , NULL, NULL, "+top?"             , "help" }
, { OPT_OP_CL       , OPTG_OP     , ACT_OPER      , ukoct::OPER_CLOSURE         , false,  0, '\0', ""      , NULL, NULL, "+cl"               , "help" }
, { OPT_OP_TCL      , OPTG_OP     , ACT_OPER      , ukoct::OPER_TIGHTCLOSURE    , false,  0, '\0', ""      , NULL, NULL, "+tcl"              , "help" }

// Unary operators
, { OPT_OP_ECHO     , OPTG_OP     , ACT_OPER      , ukoct::OPER_NONE            , false,  0, '\0', ""      , NULL, NULL, "+echo"             , "help" }
, { OPT_OP_SP       , OPTG_OP     , ACT_OPER      , ukoct::OPER_SHORTESTPATH    , false,  0, '\0', ""      , NULL, NULL, "+sp"               , "help" }
, { OPT_OP_STR      , OPTG_OP     , ACT_OPER      , ukoct::OPER_STRENGTHEN      , false,  0, '\0', ""      , NULL, NULL, "+str"              , "help" }
, { OPT_OP_TIGHT    , OPTG_OP     , ACT_OPER      , ukoct::OPER_TIGHTEN         , false,  0, '\0', ""      , NULL, NULL, "+tight"            , "help" }
, { OPT_OP_TOP      , OPTG_OP     , ACT_OPER      , ukoct::OPER_TOP             , false,  0, '\0', ""      , NULL, NULL, "+top"              , "help" }

// Binary operators
, { OPT_OP_PUSHD    , OPTG_OP     , ACT_OPER      , ukoct::OPER_PUSHDIFFCONS    , false,  1, '\0', ""      , NULL, NULL, "+pushd"            , "help" }
, { OPT_OP_PUSHC    , OPTG_OP     , ACT_OPER      , ukoct::OPER_PUSHOCTCONS     , false,  1, '\0', ""      , NULL, NULL, "+pushc"            , "help" }
, { OPT_OP_POP      , OPTG_OP     , ACT_OPER      , ukoct::OPER_FORGETOCTVAR    , false,  1, '\0', ""      , NULL, NULL, "+pop"              , "help" }

// Binary inter-matrix Operators
, { OPT_OP_EQ       , OPTG_OP     , ACT_OPER      , ukoct::OPER_EQUALS          , false,  1, '\0', ""      , NULL, NULL, "+eq"               , "help" }
, { OPT_OP_NEQ      , OPTG_OP     , ACT_OPER      , ukoct::OPER_NONE            , false,  1, '\0', ""      , NULL, NULL, "+neq"              , "help" }
, { OPT_OP_INC      , OPTG_OP     , ACT_OPER      , ukoct::OPER_INCLUDES        , false,  1, '\0', ""      , NULL, NULL, "+inc"              , "help" }
, { OPT_OP_AND      , OPTG_OP     , ACT_OPER      , ukoct::OPER_UNION           , false,  1, '\0', ""      , NULL, NULL, "+and"              , "help" }
, { OPT_OP_OR       , OPTG_OP     , ACT_OPER      , ukoct::OPER_INTERSECTION    , false,  1, '\0', ""      , NULL, NULL, "+or"               , "help" }
};


const TypeName operNames[ukoct::OPER_MAX_ + 1] {
	  { ukoct::OPER_NONE            , NULL }
	, { ukoct::OPER_COPY            , "copy" }
	, { ukoct::OPER_ISCONSISTENT    , "sat?" }
	, { ukoct::OPER_ISINTCONSISTENT , "isat?" }
	, { ukoct::OPER_ISCOHERENT      , "coh?" }
	, { ukoct::OPER_ISCLOSED        , "cl?" }
	, { ukoct::OPER_ISSTRONGLYCLOSED, "scl?" }
	, { ukoct::OPER_ISTIGHTLYCLOSED , "tcl?" }
	, { ukoct::OPER_ISWEAKLYCLOSED  , "wcl?" }
	, { ukoct::OPER_ISTOP           , "top?" }

	, { ukoct::OPER_CLOSURE         , "cl" }
	, { ukoct::OPER_TIGHTCLOSURE    , "tcl" }
	, { ukoct::OPER_SHORTESTPATH    , "sp" }
	, { ukoct::OPER_STRENGTHEN      , "str" }
	, { ukoct::OPER_TIGHTEN         , "tight" }
	, { ukoct::OPER_TOP             , "top" }

	, { ukoct::OPER_PUSHDIFFCONS    , "pushd" }
	, { ukoct::OPER_PUSHOCTCONS     , "pushc" }
	, { ukoct::OPER_FORGETOCTVAR    , "pop" }

	, { ukoct::OPER_EQUALS          , "eq" }
	, { ukoct::OPER_INCLUDES        , "inc" }
	, { ukoct::OPER_UNION           , "and" }
	, { ukoct::OPER_INTERSECTION    , "or" }
};


const OperDetailValue operDetailValues[operDetailValues_sz] = {
	  { "globalmem", ukoct::O_MEM   , ukoct::O_MEM_GLOBAL    , "Forces operation to execute totally in global memory." }
	, { "localmem" , ukoct::O_MEM   , ukoct::O_MEM_LOCAL     , "Optimize operation by executing part of it in local memory." }
	, { "exactdims", ukoct::O_DIMS  , ukoct::O_DIMS_EXACT    , "Forces the implementation to use possibly non-optimized dimensions by making it equal the problem size. In OpenCL, this means no over-provisioning of work items to favour any particular architecture or OpenCL implementation." }
	, { "fullgroup", ukoct::O_DIMS  , ukoct::O_DIMS_FULLGROUP, "Forces the implementation to use local sizes that equals the global sizes. There's a great chance of kernel launch failure if this option is given, so be conscious of the problem size and your environment's limitations." }
	, { "perelem"  , ukoct::O_REDUCE, ukoct::O_REDUCE_ELEM   , "On operators implementing any kind of reduction, this option forces the implementation to reduce the values element-wise." }
	, { "pervec"   , ukoct::O_REDUCE, ukoct::O_REDUCE_VEC    , "On operators implementing any kind of reduction, this option forces the implementation to reduce the values vector-wise. The size of the vector is implementation-dependent, but is generally 4-elements wide." }
	, { "onepass"  , ukoct::O_EXEC  , ukoct::O_EXEC_ONEPASS  , "On operations implementing third dimension loops, this option forces the implementation to make a single kernel call, without internal kernel loops. E.g.: queue(kernel(m))." }
	, { "looped"   , ukoct::O_EXEC  , ukoct::O_EXEC_LOOP     , "On operations implementing third dimension loops, this option forces the implementation to make a single kernel call, probably with internal kernel loops. E.g.: queue(kernel(m, foreach k))." }
	, { "queued"   , ukoct::O_EXEC  , ukoct::O_EXEC_QUEUED   , "On operations implementing third dimension loops, this option forces the implementation to make one kernel call per third dimension index. E.g.: foreach k do queue(kernel(m, k))." }
	, { NULL       , 0 }
};


const IOperationCallback* operationCallbacks[ukoct::IMPL_MAX_ + 1][ukoct::ELEM_MAX_ + 1] = {
	//                  NONE, HALF, FLOAT         , DOUBLE     , LDOUBLE
	/* IMPL_NONE   */ { NULL, NULL, NULL          , NULL       , NULL },
	/* IMPL_CPU    */ { NULL, NULL, ocb_cpu_flt   , ocb_cpu_dbl, ocb_cpu_ldbl },
	/* IMPL_OPENCL */ { NULL, NULL, ocb_opencl_flt, NULL       , NULL }
};


const TypeName elemNames[ukoct::ELEM_MAX_ + 1] = {
	{ ukoct::ELEM_NONE   , NULL        },
	{ ukoct::ELEM_HALF   , "half"      },
	{ ukoct::ELEM_FLOAT  , "float"     },
	{ ukoct::ELEM_DOUBLE , "double"    },
	{ ukoct::ELEM_LDOUBLE, "ldouble"   }
};


const TypeName implNames[ukoct::IMPL_MAX_ + 1] = {
	{ ukoct::IMPL_NONE  , NULL        },
	{ ukoct::IMPL_CPU   , "cpu"       },
	{ ukoct::IMPL_OPENCL, "opencl"    }
};
