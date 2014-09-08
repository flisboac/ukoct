#ifndef UKOCT_OPENCL_COMMON_OPERATORS_TIGHTLYCLOSED_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_TIGHTLYCLOSED_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct TightlyClosedOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct TightlyClosedOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_stronglyClosed__global_reduce"; }
};


template <typename T> class TightlyClosedOperator_GlobalReduce :
	public AbstractGlobalAuxOperator<T, TightlyClosedOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::TightlyClosedOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	cl::NDRange globalNDRange(typename IOperator<T>::Args& args) { return args.state().globalRange3(); }
	ukoct::EOperation operation() { return ukoct::OPER_TIGHTLYCLOSED; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_TIGHTLYCLOSED_HPP_ */
