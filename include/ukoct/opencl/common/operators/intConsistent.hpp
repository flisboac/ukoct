#ifndef UKOCT_OPENCL_COMMON_OPERATORS_INTCONSISTENT_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_INTCONSISTENT_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct IntConsistentOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalReduceFunctionName() { return ""; }
};


template <> struct IntConsistentOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalReduceFunctionName() { return "octdiff_intConsistent__global_reduce"; }
};

template <typename T> class IntConsistentOperator_GlobalReduce :
		public AbstractGlobalAuxOperator<T, IntConsistentOperator_GlobalReduce<T> > {
public:
	typedef impl::opencl::octdiff::IntConsistentOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalReduceFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_INTCONSISTENT; }
	cl::NDRange globalNDRange(typename IOperator<T>::Args& args) { return args.state().globalRange1(); }
};

}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_INTCONSISTENT_HPP_ */
