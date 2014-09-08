#ifndef UKOCT_OPENCL_COMMON_OPERATORS_COPY_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_COPY_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct CopyOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct CopyOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "?"; }
};


template <typename T> class CopyOperator_Global :
	public AbstractGlobalAuxOperator<T, CopyOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::CopyOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_COPY; }
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_COPY_HPP_ */
