#ifndef UKOCT_CPU_OPERATORS_ISCOHERENT_HPP_
#define UKOCT_CPU_OPERATORS_ISCOHERENT_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_ISCOHERENT

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class IsCoherentCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef IsCoherentCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT; }
};


template <typename T> class IsCoherentCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		ret.boolResult = true;

		for (plas::var_t i = 1; i <= state.diffSize() && ret.boolResult; ++i) {
			for (plas::var_t j = 1; i <= state.diffSize(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				if (mat(i, j) != mat(J, I))
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

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_ISCOHERENT_HPP_ */
