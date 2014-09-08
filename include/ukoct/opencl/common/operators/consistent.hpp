#ifndef UKOCT_OPENCL_COMMON_OPERATORS_CONSISTENT_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_CONSISTENT_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct ConsistentOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct ConsistentOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_consistent__global_reduce"; }
};

template <typename T> class ConsistentOperator_Global :
		public AbstractGlobalAuxOperator<T, ConsistentOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::ConsistentOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_CONSISTENT; }
	cl::NDRange globalNDRange(typename IOperator<T>::Args& args) { return args.state().globalRange1(); }
};

}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_CONSISTENT_HPP_ */
