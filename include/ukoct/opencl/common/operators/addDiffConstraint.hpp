#ifndef UKOCT_OPENCL_COMMON_OPERATORS_ADDDIFFCONSTRAINT_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_ADDDIFFCONSTRAINT_HPP_

#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct AddDiffConstraintOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct AddDiffConstraintOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "octdiff_addDiffConstraint__global"; }
};


template <typename T> class AddDiffConstraintOperator_Global :
	public AbstractGlobalOperator<T, AddDiffConstraintOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::AddDiffConstraintOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_ADDDIFFCONS; }
	cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel = AbstractGlobalOperator<T, AddDiffConstraintOperator_Global<T> >::createKernel(args, nextArgIdx);
		kernel.setArg(nextArgIdx++, args.diffcons().a());
		kernel.setArg(nextArgIdx++, args.diffcons().b());
		kernel.setArg(nextArgIdx++, args.diffcons().d());
		return kernel;
	}
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_ADDDIFFCONSTRAINT_HPP_ */
