#ifndef UKOCT_CPU_OPERATORS_CLOSURE_HPP_
#define UKOCT_CPU_OPERATORS_CLOSURE_HPP_

#include "ukoct/cpu/operators/abstract.hpp"
#include "ukoct/cpu/operators/shortestPath.hpp"
#include "ukoct/cpu/operators/isConsistent.hpp"
#include "ukoct/cpu/operators/strengthen.hpp"

#define ukoct_OPERCODE ukoct::OPER_CLOSURE

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class ClosureCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef ClosureCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};


template <typename T> class ClosureCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ShortestPathCpuOperator<T> shortestPath;
		IsConsistentCpuOperator<T> isConsistent;
		StrengthenCpuOperator<T> strengthen;

		shortestPath.run(args, ret);
		isConsistent.run(args, ret);

		if (ret.boolResult)
			strengthen.run(args, ret);

		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}

#undef ukoct_OPERCODE

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_CLOSURE_HPP_ */
