#ifndef UKOCT_CPU_STATE_HPP_
#define UKOCT_CPU_STATE_HPP_

#include <algorithm>

#include "ukoct/cpu/base.hpp"
#include "ukoct/cpu/operators.hpp"

namespace ukoct{

template <typename T> class CpuState : public IState<T> {
public:
	const IImplementation<T>& implementation() const { return cpu<T>::impl; }
	CpuState<T>* clone() const { return new CpuState<T>(); }
	bool isValid() const { return _valid; }
	size_t diffSize() const { return _self.colsize(); }
	bool rowMajor() const { return _self.ordering() == plas::MATRIX_ROWMAJOR ? true : false; }
	void copyTo(T* ptr) const { std::copy(_input.begin(), _input.end(), ptr); }
	void setup(size_t diffSize, T* rawInput, bool rowMajor, void* implOptions = NULL) {
		assertStateOptions(_valid, diffSize, rawInput, rowMajor);
		_self = plas::DenseMatrix<T>(diffSize, diffSize, rowMajor ? plas::MATRIX_ROWMAJOR : plas::MATRIX_COLMAJOR, implementation().infinity());
		std::copy(rawInput, rawInput + (diffSize * diffSize), _self.raw());
		_valid = true;
	}

	plas::DenseMatrix<T>& self() { return _self; }

private:
	bool _valid;
	plas::DenseMatrix<T> _self;
};

template <typename T> class CpuImplementation : public IImplementation<T> {
public:
	inline bool isImplemented() const { return true; }
	inline EImplementation type() const { return IMPL_CPU; }
	inline IState<T>* newState() const { return new CpuState<T>(); }
	inline const IOperator<T> * const* getOperators(size_t& numOperators) const { return _operators; }

private:
	static const impl::cpu::CopyCpuOperator<T>             copyOperator;
	static const impl::cpu::IsConsistentCpuOperator<T>     isConsistentOperator;
	static const impl::cpu::IsIntConsistentCpuOperator<T>  isIntConsistentOperator;
	static const impl::cpu::IsCoherentCpuOperator<T>       isCoherentOperator;
	static const impl::cpu::IsClosedCpuOperator<T>         isClosedOperator;
	static const impl::cpu::IsStronglyClosedCpuOperator<T> isStronglyClosedOperator;
	static const impl::cpu::IsTightlyClosedCpuOperator<T>  isTightlyClosedOperator;
	static const impl::cpu::IsWeaklyClosedCpuOperator<T>   isWeaklyClosedOperator;
	static const impl::cpu::IsTopCpuOperator<T>            isTopOperator;

	static const impl::cpu::ClosureCpuOperator<T>      closureOperator;
	static const impl::cpu::TightClosureCpuOperator<T> tightClosureOperator;
	static const impl::cpu::ShortestPathCpuOperator<T> shortestPathOperator;
	static const impl::cpu::StrengthenCpuOperator<T>   strengthenOperator;
	static const impl::cpu::TightenCpuOperator<T>      tightenOperator;
	static const impl::cpu::TopCpuOperator<T>          topOperator;

	static const impl::cpu::PushDiffConsCpuOperator<T> pushDiffConsOperator;
	static const impl::cpu::PushOctConsCpuOperator<T>  pushOctConsOperator;
	static const impl::cpu::ForgetOctVarCpuOperator<T> forgetOctVarOperator;

	static const impl::cpu::EqualsCpuOperator<T>       equalsOperator;
	static const impl::cpu::IncludesCpuOperator<T>     includesOperator;
	static const impl::cpu::UnionCpuOperator<T>        unionOperator;
	static const impl::cpu::IntersectionCpuOperator<T> intersectionOperator;

	static const IOperator<T>* const _operators[] = {
		copyOperator
		, isConsistentOperator
		, isIntConsistentOperator
		, isCoherentOperator
		, isClosedOperator
		, isStronglyClosedOperator
		, isTightlyClosedOperator
		, isWeaklyClosedOperator
		, isTopOperator

		, closureOperator
		, tightClosureOperator
		, shortestPathOperator
		, strengthenOperator
		, tightenOperator
		, topOperator

		, pushDiffConsOperator
		, pushOctConsOperator
		, forgetOctVarOperator

		, equalsOperator
		, includesOperator
		, unionOperator
		, intersectionOperator
	};
};

template <typename T> struct cpu {
	static const CpuImplementation<T> impl;
};

}


#endif /* UKOCT_CPU_STATE_HPP_ */
