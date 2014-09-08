#ifndef UKOCT_OPENCL_COMMON_OPERATORS_SHORTESTPATH_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_SHORTESTPATH_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct ShortestPathOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalLoopFunctionName() { return ""; }
};


template <> struct ShortestPathOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalLoopFunctionName() { return "octdiff_shortestPath__global_loop"; }
};


template <typename T> class ShortestPathOperator_GlobalLoop :
	public AbstractGlobalOperator<T, ShortestPathOperator_GlobalLoop<T> > {
public:
	typedef impl::opencl::octdiff::ShortestPathOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalLoopFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_SHORTESTPATH; }
	cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel = AbstractGlobalOperator<T, ShortestPathOperator_GlobalLoop<T> >::createKernel(args, nextArgIdx);
		kernel.setArg(nextArgIdx++, args.aux());
		kernel.setArg(nextArgIdx++, args.size());
		return kernel;
	}
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_SHORTESTPATH_HPP_ */
