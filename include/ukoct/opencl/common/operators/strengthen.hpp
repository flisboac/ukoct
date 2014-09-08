#ifndef UKOCT_OPENCL_COMMON_OPERATORS_STRENGTHEN_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_STRENGTHEN_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct StrengthenOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct StrengthenOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "octdiff_strengthen__global"; }
};


template <typename T> class StrengthenOperator_Global :
	public AbstractGlobalOperator<T, StrengthenOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::StrengthenOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_STRENGTHEN; }
	cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel = AbstractGlobalOperator<T, StrengthenOperator_Global<T> >::createKernel(args, nextArgIdx);
		kernel.setArg(nextArgIdx++, args.intBased());
		return kernel;
	}
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_STRENGTHEN_HPP_ */
