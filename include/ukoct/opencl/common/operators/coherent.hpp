#ifndef UKOCT_OPENCL_COMMON_OPERATORS_COHERENT_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_COHERENT_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct CoherentOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct CoherentOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_coherent__global_reduce"; }
};


template <typename T> class CoherentOperator_GlobalReduce :
		public AbstractGlobalAuxOperator<T, CoherentOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::CoherentOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_COHERENT; }
};

}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_COHERENT_HPP_ */
