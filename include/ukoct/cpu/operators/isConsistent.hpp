#ifndef UKOCT_CPU_OPERATORS_ISCONSISTENT_HPP_
#define UKOCT_CPU_OPERATORS_ISCONSISTENT_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class IsConsistentCpuOperator : public AbstractCpuOperator<T> {
public:
	static inline constexpr ukoct::EOperation getOperation() { return OPER_ISCONSISTENT; }
	static inline constexpr ukoct::OperationDetails getDetails() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }

	ukoct::EOperation operation() const { return getOperation(); }
	ukoct::OperationDetails details() const { return getDetails(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		ret.boolResult = true;

		for (plas::var_t k = 1; k <= state.diffSize() && ret.boolResult; ++k) {
			if (mat(k, k) < 0)
				ret.boolResult = false;
			else
				mat(k, k) = 0;
		}

		AbstractCpuOperator<T>::end(timing);
	}
};


}
}
}
}


#endif /* UKOCT_CPU_OPERATORS_ISCONSISTENT_HPP_ */
