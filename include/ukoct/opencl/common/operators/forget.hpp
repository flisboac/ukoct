#ifndef UKOCT_OPENCL_COMMON_OPERATORS_FORGET_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_FORGET_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct ForgetOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct ForgetOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "octdiff_forget__global"; }
};


template <typename T> class ForgetOperator_Global :
	public AbstractGlobalOperator<T, ForgetOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::ForgetOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_FORGET; }
	cl::NDRange globalNDRange(typename IOperator<T>::Args& args) { return cl::NDRange(2, 2); }
	cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel = AbstractGlobalOperator<T, ForgetOperator_Global<T> >::createKernel(args, nextArgIdx);
		kernel.setArg(nextArgIdx++, args.state().size());
		kernel.setArg(nextArgIdx++, args.var());
		return kernel;
	}
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_FORGET_HPP_ */
