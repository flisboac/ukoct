#ifndef UKOCT_OPENCL_COMMON_OPERATORS_WEAKLYCLOSED_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_WEAKLYCLOSED_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct WeaklyClosedOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct WeaklyClosedOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_consistent__global_reduce"; }
};


template <typename T> class WeaklyClosedOperator_GlobalReduce :
	public AbstractGlobalAuxOperator<T, WeaklyClosedOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::WeaklyClosedOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_WEAKLYCLOSED; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_WEAKLYCLOSED_HPP_ */
