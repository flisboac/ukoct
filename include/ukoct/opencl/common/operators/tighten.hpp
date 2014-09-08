#ifndef UKOCT_OPENCL_COMMON_OPERATORS_TIGHTEN_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_TIGHTEN_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct TightenOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct TightenOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_tighten__global"; }
};

template <typename T> class TightenOperator_Global :
		public AbstractGlobalAuxOperator<T, TightenOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::TightenOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_TIGHTEN; }
	cl::NDRange globalNDRange(typename IOperator<T>::Args& args) { return args.state().globalRange1(); }
};

}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_TIGHTEN_HPP_ */
