#ifndef UKOCT_OPENCL_COMMON_OPERATORS_EQUALS_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_EQUALS_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct EqualsOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct EqualsOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_equals__global_reduce"; }
};


template <typename T> class EqualsOperator_GlobalReduce :
	public AbstractGlobalAuxBinaryOperator<T, EqualsOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::EqualsOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_EQUALS; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_EQUALS_HPP_ */
