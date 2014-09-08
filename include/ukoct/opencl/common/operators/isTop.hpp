#ifndef UKOCT_OPENCL_COMMON_OPERATORS_ISTOP_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_ISTOP_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct IsTopOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct IsTopOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_isTop__global_reduce"; }
};


template <typename T> class IsTopOperator_GlobalReduce :
	public AbstractGlobalAuxOperator<T, IsTopOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::IsTopOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_ISTOP; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_ISTOP_HPP_ */
