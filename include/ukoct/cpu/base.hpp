#ifndef UKOCT_CPU_BASE_HPP_
#define UKOCT_CPU_BASE_HPP_

namespace ukoct {

template <typename T> class CpuImplementation;
template <typename T> class CpuState;

namespace impl {
namespace cpu {
namespace octdiff {

template <typename T> class NopCpuOperator;

template <typename T, ukoct::EOperation Oper> struct OperationImpl {
	static constexpr bool valid = false;
	typedef NopCpuOperator<T> Impl;
	typedef T Type;
	static constexpr ukoct::EOperation operation = Oper;
	static constexpr ukoct::OperationDetails details() { return 0; }
};

}
}
}

}

#endif /* UKOCT_CPU_BASE_HPP_ */
