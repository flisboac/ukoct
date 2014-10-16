#ifndef UKOCT_CPU_OPERATORS_SHORTESTPATH_HPP_
#define UKOCT_CPU_OPERATORS_SHORTESTPATH_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_SHORTESTPATH

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class ShortestPathCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef ShortestPathCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};

template <typename T> class ShortestPathCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		size_t iters = args.iterations();

		if (iters == 0)
			iters = state.diffSize();

		if (iters > state.diffSize() / 2)
			iters = state.diffSize() / 2;

		for (size_t k = 1; k <= iters; ++k, ++k) {
			plas::var_t K = plas::switchVar(k);

			for (plas::var_t i = 1; i <= state.diffSize(); ++i)
				for (plas::var_t j = 1; j <= state.diffSize(); ++j) {
					plas::var_t I = plas::switchVar(i);
					plas::var_t J = plas::switchVar(j);
					mat(i, j) = std::min(
						mat(i, j), std::min(                            // i -> j
							mat(i, k) + mat(k, j), std::min(            // i -> k -> j
								mat(i, K) + mat(K, j), std::min(        // i -> K -> j
									mat(i, k) + mat(k, K) + mat(K, j),  // i -> k -> K -> j
									mat(i, K) + mat(K, k) + mat(k, j)   // i -> K -> k -> j
								)
							)
						)
					);
				}

			// QUESTION: Is it needed to strengthen right after shortest path, as in miné's paper,
			// or can it be left to closure?
			// ANSWER: YES.
			// TODO strengthen here.
		}

		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}

#undef ukoct_OPERCODE

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_SHORTESTPATH_HPP_ */
