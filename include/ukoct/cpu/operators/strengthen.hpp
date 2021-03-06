#ifndef UKOCT_CPU_OPERATORS_STRENGTHEN_HPP_
#define UKOCT_CPU_OPERATORS_STRENGTHEN_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_STRENGTHEN

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class StrengthenCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef StrengthenCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};

template <typename T> class StrengthenCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		bool intBased = args.intBased();

		for (plas::var_t i = 1; i <= state.diffSize(); ++i) {
			for (plas::var_t j = 1; j <= state.diffSize(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);
				if (!intBased)
					mat(i, j) = std::min(mat(i, j), (mat(i, I) + mat(J, j)) / 2);

				else {
					if (i == j)
						mat(i, j) = std::min(mat(i, j), std::floor((mat(i, I) + mat(J, j)) / 2));
					else
						mat(i, j) = 2 * std::floor(mat(i, I) / 2);
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

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_STRENGTHEN_HPP_ */
