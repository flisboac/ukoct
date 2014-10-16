#ifndef UKOCT_CPU_OPERATORS_ISWEAKLYCLOSED_HPP_
#define UKOCT_CPU_OPERATORS_ISWEAKLYCLOSED_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

#define ukoct_OPERCODE ukoct::OPER_ISWEAKLYCLOSED

template <typename T> class IsWeaklyClosedCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef IsWeaklyClosedCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};


template <typename T> class IsWeaklyClosedCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

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

		for (plas::var_t i = 1; i <= state.diffSize() && ret.boolResult; ++i) {
			for (plas::var_t j = 1; j <= state.diffSize() && ret.boolResult; ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				for (plas::var_t k = 1; k <= state.diffSize() && ret.boolResult; ++k) {
					if (mat(i, k) + mat(k, j) < std::min(mat(i, j), (mat(i, I) + mat(J, j)) / 2))
						ret.boolResult = false;
				}
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

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_ISWEAKLYCLOSED_HPP_ */
