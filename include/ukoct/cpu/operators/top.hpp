#ifndef UKOCT_CPU_OPERATORS_TOP_HPP_
#define UKOCT_CPU_OPERATORS_TOP_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_TOP

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class TopCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef TopCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};

template <typename T> class TopCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		T infinity = state.implementation().infinity();

		for (plas::var_t i = 1; i <= state.diffSize(); ++i) {
			for (plas::var_t j = 1; j <= state.diffSize(); ++j)
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

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_TOP_HPP_ */
