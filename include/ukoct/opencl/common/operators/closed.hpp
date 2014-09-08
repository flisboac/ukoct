#ifndef UKOCT_OPENCL_COMMON_OPERATORS_CLOSED_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_CLOSED_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct ClosedOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct ClosedOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_closed__global_reduce"; }
};


template <typename T> class ClosedOperator_GlobalReduce :
	public AbstractGlobalAuxOperator<T, ClosedOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::ClosedOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	cl::NDRange globalNDRange(typename IOperator<T>::Args& args) { return args.state().globalRange3(); }
	ukoct::EOperation operation() { return ukoct::OPER_ISCLOSED; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_CLOSED_HPP_ */
