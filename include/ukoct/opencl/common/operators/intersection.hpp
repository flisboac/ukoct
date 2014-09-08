#ifndef UKOCT_OPENCL_COMMON_OPERATORS_INTERSECTION_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_INTERSECTION_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct IntersectionOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct IntersectionOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "octdiff_intersection__global"; }
};


template <typename T> class IntersectionOperator_Global :
	public AbstractGlobalBinaryOperator<T, IntersectionOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::IntersectionOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_INTERSECTION; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_INTERSECTION_HPP_ */
