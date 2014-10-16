#ifndef UKOCT_CPU_OPERATORS_TIGHTCLOSURE_HPP_
#define UKOCT_CPU_OPERATORS_TIGHTCLOSURE_HPP_

#include "ukoct/cpu/operators/abstract.hpp"
#include "ukoct/cpu/operators/shortestPath.hpp"
#include "ukoct/cpu/operators/isConsistent.hpp"
#include "ukoct/cpu/operators/isIntConsistent.hpp"
#include "ukoct/cpu/operators/tighten.hpp"

#define ukoct_OPERCODE ukoct::OPER_TIGHTCLOSURE

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class TightClosureCpuOperator;

template <typename T> struct OperationImpl<T, ukoct_OPERCODE> {
	static constexpr bool valid = true;
	typedef TightClosureCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = ukoct_OPERCODE;
	static constexpr ukoct::OperationDetails details() { return O_EXEC_LOOP | O_DIMS_EXACT | O_IMPL_MUTATOR; }
};

template <typename T> class TightClosureCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OperationImpl<T, ukoct_OPERCODE>::operation; }
	ukoct::OperationDetails details() const { return OperationImpl<T, ukoct_OPERCODE>::details(); }

	void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);

		ShortestPathCpuOperator<T> shortestPath;
		IsConsistentCpuOperator<T> isConsistent;
		IsIntConsistentCpuOperator<T> isIntConsistent;
		TightenCpuOperator<T> tighten;

		shortestPath.run(args, ret);
		isConsistent.run(args, ret);

		if (ret.boolResult) {
			tighten.run(args, ret);
			isIntConsistent.run(args, ret);
		}

		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}

#undef ukoct_OPERCODE

#endif /* UKOCT_OPENCL_COMMON_OPERATORS_CLOSURE_HPP_ */
