#ifndef UKOCT_CPU_OPERATORS_FORGETOCTVAR_HPP_
#define UKOCT_CPU_OPERATORS_FORGETOCTVAR_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_FORGETOCTVAR

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class ForgetOctVarCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef ForgetOctVarCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};


template <typename T> class ForgetOctVarCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		T infinity = state.implementation().infinity();
		plas::var_t v = plas::normalizeVar(args.var());

		ukoct_ASSERT(v > 0 && v <= state.octSize(), "Variable out of range, should be in between 1 and octSize.");

		plas::var_t min = 2 * v - 1;
		plas::var_t max = 2 * v;

		// Would be wrong if it wasn't know beforehand that each variable loop
		// will be run exactly twice.
		for (plas::var_t i = min; i <= max; ++i) {
			for (plas::var_t j = min; j <= max; ++j)
				if (i == j)
					mat(i, j) = 0;
				else
					mat(i, j) = infinity;
		}

		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}

#undef ukoct_OPERCODE

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_FORGETOCTVAR_HPP_ */
