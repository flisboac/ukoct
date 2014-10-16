#ifndef UKOCT_CPU_OPERATORS_NOP_HPP_
#define UKOCT_CPU_OPERATORS_NOP_HPP_

#include "ukoct/cpu/operators/abstract.hpp"

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class NopCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_NONE; }
	ukoct::OperationDetails details() const { return 0; }

	void run(const OperatorArgs<T>& args) {
		CpuTiming timing;
		AbstractCpuOperator<T>::start(timing);
		AbstractCpuOperator<T>::end(timing);
	}
};

}
}
}
}


#endif /* UKOCT_CPU_OPERATORS_NOP_HPP_ */
