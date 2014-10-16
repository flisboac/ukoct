#ifndef UKOCT_CPU_OPERATORS_ISCLOSED_HPP_
#define UKOCT_CPU_OPERATORS_ISCLOSED_HPP_

#include "ukoct/cpu/operators/abstract.hpp"
#include "ukoct/cpu/operators/isConsistent.hpp"

#define ukoct_OPERCODE ukoct::OPER_ISCLOSED

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class IsClosedCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef IsClosedCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};


template <typename T> class IsClosedCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		IsConsistentCpuOperator<T> isConsistent;
		isConsistent.run(args, ret);

		if (ret.boolResult) {
			ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
			plas::DenseMatrix<T>& mat = state.input();

			for (plas::var_t i = 1; i <= state.diffSize() && ret.boolResult; ++i) {
				for (plas::var_t j = 1; j <= state.diffSize() && ret.boolResult; ++j)
					for (plas::var_t k = 1; k <= state.diffSize() && ret.boolResult; ++k)
						if (mat(i, j) > mat(i, k) + mat(k, j))
							ret.boolResult = false;
			}
		}

		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}

#undef ukoct_OPERCODE

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_ISCLOSED_HPP_ */
