#ifndef UKOCT_OPENCL_COMMON_OPERATORS_TOP_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_TOP_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct TopOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct TopOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "octdiff_top__global"; }
};


template <typename T> class TopOperator_Global :
	public AbstractGlobalAuxOperator<T, TopOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::TopOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_TOP; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_TOP_HPP_ */
