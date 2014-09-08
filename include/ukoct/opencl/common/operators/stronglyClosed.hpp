#ifndef UKOCT_OPENCL_COMMON_OPERATORS_STRONGLYCLOSED_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_STRONGLYCLOSED_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct StronglyClosedOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct StronglyClosedOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_stronglyClosed__global_reduce"; }
};


template <typename T> class StronglyClosedOperator_GlobalReduce :
	public AbstractGlobalAuxOperator<T, StronglyClosedOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::StronglyClosedOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	cl::NDRange globalNDRange(typename IOperator<T>::Args& args) { return args.state().globalRange3(); }
	ukoct::EOperation operation() { return ukoct::OPER_STRONGLYCLOSED; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_STRONGLYCLOSED_HPP_ */
