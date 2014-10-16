#ifndef UKOCT_CPU_OPERATORS_PUSHOCTCONS_HPP_
#define UKOCT_CPU_OPERATORS_PUSHOCTCONS_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

#define ukoct_OPERCODE ukoct::OPER_PUSHOCTCONS

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class PushOctConsCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef PushOctConsCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};


template <typename T> class PushOctConsCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::OctDiffConstraint<T> ca, cb;
		plas::DenseMatrix<T>& mat = state.input();
		args.octCons().split(ca, cb);

		for (plas::var_t i = 1; i < state.diffSize(); ++i) {
			for (plas::var_t j = 1; j < state.diffSize(); ++j) {
				T da = mat(i, ca.a()) + ca.d() + mat(ca.b(), j);
				T db = da;
				if (cb.valid())
					db = mat(i, cb.a()) + cb.d() + mat(cb.b(), j);
				mat(i, j) = std::min(mat(i, j), std::min(da, db));
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

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_PUSHOCTCONS_HPP_ */
