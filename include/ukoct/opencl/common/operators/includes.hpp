#ifndef UKOCT_OPENCL_COMMON_OPERATORS_INCLUDES_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_INCLUDES_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct IncludesOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct IncludesOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_includes__global_reduce"; }
};


template <typename T> class IncludesOperator_GlobalReduce :
	public AbstractGlobalAuxBinaryOperator<T, IncludesOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::IncludesOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_INCLUDES; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_INCLUDES_HPP_ */
