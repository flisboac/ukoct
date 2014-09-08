#ifndef UKOCT_OPENCL_COMMON_OPERATORS_UNION_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_UNION_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct UnionOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct UnionOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "?"; }
};


template <typename T> class UnionOperator_Global :
	public AbstractGlobalAuxOperator<T, UnionOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::UnionOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_UNION; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_UNION_HPP_ */
