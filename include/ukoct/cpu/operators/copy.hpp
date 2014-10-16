#ifndef UKOCT_CPU_OPERATORS_COPY_HPP_
#define UKOCT_CPU_OPERATORS_COPY_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_COPY

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class CopyCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef CopyCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};


template <typename T> class CopyCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ukoct_ASSERT(args.other() != NULL, "Secondary matrix must be provided for this operator.");
		ukoct_ASSERT(args.state().implementation() != args.other()->implementation(), "Both matrices need to be from the same implementation.");
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		ukoct::CpuState<T>& other = *reinterpret_cast<CpuState<T>*>(args.other());
		plas::DenseMatrix<T>& mat = state.input();
		plas::DenseMatrix<T>& oth = other.input();
		std::copy(oth.raw(), oth.raw() + (state.diffSize() * state.diffSize()), mat.raw());

		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}

#undef ukoct_OPERCODE

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_COPY_HPP_ */
