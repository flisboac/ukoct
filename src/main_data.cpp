#include "main.hpp"
#include "main_general.hpp"
#include "main_details.hpp"
#include "main_oper.hpp"

NullStream nullstream;


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


const VariantDefinition variantDefinitions[variantDefinitions_sz] = {
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


const IOptionGroupCallback* optionGroupCallbacks[OPTG_MAX_ + 1] = {
	NULL,
	general_operationCallback,
	details_operationCallback,
	oper_operationCallback,
};

