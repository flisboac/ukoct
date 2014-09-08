#ifndef UKOCT_OPENCL_COMMON_OPERATORS_ADDCONSTRAINT_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_ADDCONSTRAINT_HPP_

#include "plas.hpp"
#include "ukoct/opencl/common/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T> struct AddConstraintOperatorImplData {
	static const bool specialized = false;
	static constexpr const char* globalFunctionName() { return ""; }
};


template <> struct AddConstraintOperatorImplData<cl_float> {
	static const bool specialized = true;
	static constexpr const char* globalFunctionName() { return "octdiff_addConstraint__global"; }
};


template <typename T> class AddConstraintOperator_Global :
	public AbstractGlobalOperator<T, AddConstraintOperator_Global<T> > {
public:
	typedef impl::opencl::octdiff::AddConstraintOperatorImplData<T> ImplData;
	const char* kernelFunctionName() { return ImplData::globalFunctionName(); }
	ukoct::EOperation operation() { return ukoct::OPER_ADDOCTCONS; }
	cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		plas::OctDiffConstraint<T> ca;
		plas::OctDiffConstraint<T> cb;
		args.cons().split(ca, cb);
		cl::Kernel kernel = AbstractGlobalOperator<T, AddConstraintOperator_Global<T> >::createKernel(args, nextArgIdx);
		kernel.setArg(nextArgIdx++, ca.a());
		kernel.setArg(nextArgIdx++, ca.b());
		kernel.setArg(nextArgIdx++, ca.d());
		kernel.setArg(nextArgIdx++, cb.a());
		kernel.setArg(nextArgIdx++, cb.b());
		kernel.setArg(nextArgIdx++, cb.d());
		return kernel;
	}
};



}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_ADDCONSTRAINT_HPP_ */
