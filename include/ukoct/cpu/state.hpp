#ifndef UKOCT_CPU_STATE_HPP_
#define UKOCT_CPU_STATE_HPP_

#include <algorithm>
#include "plas.hpp"

#include "ukoct/cpu/base.hpp"
#include "ukoct/cpu/operators.hpp"

namespace ukoct{

template <typename T> class CpuState : public IState<T> {
public:
	CpuState() :
		_valid(false),
		_impl(NULL),
		_selfImpl(NULL),
		_self() {}


	CpuState(const CpuState<T>& other)  :
		_valid(other._valid),
		_impl(other._impl),
		_selfImpl(other._selfImpl),
		_self(other._self) {}


	CpuState(const CpuImplementation<T>* impl) :
		_valid(false),
		_impl(impl),
		_selfImpl(NULL),
		_self() {}


	CpuState(CpuImplementation<T>* impl)  :
		_valid(false),
		_impl(impl),
		_selfImpl(impl),
		_self() {}


	~CpuState() {
		delete _selfImpl;
	}


	const IImplementation<T>& implementation() const {
		ukoct::assert(_impl != NULL, "Missing internal IImplementation pointer.");
		return *_impl;
	}


	CpuState<T>* clone() const {
		return new CpuState<T>(*this);
	}


	bool isValid() const {
		return _impl != NULL && _valid;
	}


	size_t implSize() const {
		return _self.colsize();
	}


	bool rowMajor() const {
		return _self.ordering() == plas::MATRIX_ROWMAJOR ? true : false;
	}


	void copyTo(T* ptr) const {
		std::copy(_self.raw(), _self.raw() + (implSize() * implSize()), ptr);
	}


	plas::DenseMatrix<T>& input() {
		return _self;
	}


	void setup(size_t diffSize, T* rawInput, bool rowMajor) {
		assertStateOptions(_valid, diffSize, rawInput, rowMajor);
		_self = plas::DenseMatrix<T>(diffSize, diffSize, rowMajor ? plas::MATRIX_ROWMAJOR : plas::MATRIX_COLMAJOR, implementation().infinity());
		std::copy(rawInput, rawInput + (diffSize * diffSize), _self.raw());
		_valid = true;
	}


	plas::DenseMatrix<T>& self() {
		return _self;
	}

private:
	bool _valid;
	ukoct::CpuImplementation<T>* _selfImpl;
	const ukoct::CpuImplementation<T>* _impl;
	plas::DenseMatrix<T> _self;
};


template <typename T> class CpuImplementation : public IImplementation<T> {
public:
	EImplementation type() const {
		return IMPL_CPU;
	}


	bool intBased() const {
		return ElemTypeInfo<T>::intBased;
	}


	EElemType elemType() const {
		return ElemTypeInfo<T>::elemType;
	}


	T infinity() const {
		return ElemTypeInfo<T>::infinity();
	}


	CpuState<T>* newState() const {
		return new CpuState<T>(this);
	}


	void getDetails(std::map<EOperation, std::vector<OperationDetails> >& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false) const {

	}


	void filterOperators(std::vector<IOperator<T>*>& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false) const {
		// TODO Implementation
	}
};

namespace impl {
namespace cpu {

extern const CpuImplementation<float> floatImpl;
extern const CpuImplementation<double> doubleImpl;
extern const CpuImplementation<long double> ldoubleImpl;

}
}


template <typename T> struct ImplementationInfo<T, IMPL_CPU> {
	static constexpr bool valid = true;
	static constexpr EImplementation type = IMPL_CPU;
	static constexpr bool intBased = ElemTypeInfo<T>::intBased;
	static constexpr EElemType elemType = ElemTypeInfo<T>::elemType;
	static constexpr size_t elemSize = ElemTypeInfo<T>::elemSize;
	static T infinity() { return ElemTypeInfo<T>::infinity(); }
	static T floor(T n) { return ElemTypeInfo<T>::floor(n); }
	static T mod(T n, T d) { return ElemTypeInfo<T>::mod(n, d); }
	static IImplementation<T>* newImplementation() { return new CpuImplementation<T>(); }
	static const IImplementation<T>* implementation() {
		switch(elemType) {
		case ELEM_FLOAT  : return impl::cpu::floatImpl  ; break;
		case ELEM_DOUBLE : return impl::cpu::doubleImpl ; break;
		case ELEM_LDOUBLE: return impl::cpu::ldoubleImpl; break;
		}
		return NULL;
	}
};


}

namespace ukoct {
namespace impl {
namespace cpu {

const ukoct::CpuImplementation<float> floatImpl;
const ukoct::CpuImplementation<double> doubleImpl;
const ukoct::CpuImplementation<long double> ldoubleImpl;

}
}
}


#endif /* UKOCT_CPU_STATE_HPP_ */
