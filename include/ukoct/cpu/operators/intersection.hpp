#ifndef UKOCT_CPU_OPERATORS_INTERSECTION_HPP_
#define UKOCT_CPU_OPERATORS_INTERSECTION_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_INTERSECTION

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class IntersectionCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef IntersectionCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};


template <typename T> class IntersectionCpuOperator : public AbstractCpuOperator<T> {
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
		ret.boolResult = true;

		if (&state != &other) {
			if (state.diffSize() != other.diffSize())
				//ret.boolResult = false;
				throw Error("Problem sizes cannot be different.");

			for (plas::var_t i = 1; i <= state.diffSize() && ret.boolResult; ++i)
				for (plas::var_t j = 1; j <= state.diffSize() && ret.boolResult; ++j)
					mat(i, j) = std::min(mat(i, j), oth(i, j));
		}

		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}

#undef ukoct_OPERCODE

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_INTERSECTION_HPP_ */
