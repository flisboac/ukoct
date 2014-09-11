#ifndef UKOCT_CORE_INTERFACE_HPP_
#define UKOCT_CORE_INTERFACE_HPP_

#include "ukoct/core/defs.hpp"
#include "plas.hpp"


namespace ukoct {

template <typename T> struct IImplementation;
template <typename T> struct IState;
template <typename T> struct IOperator;
template <typename T> struct IOperatorResult;

template <typename T> class OperatorArgs {
public:
	OperatorArgs()
		: _state()
		, _other(NULL)
		, _intBased(false)
		, _waiting(true)
		, _iterations(0)
		, _var(0)
		, _diffCons()
		, _octCons()
		{}
	OperatorArgs(const OperatorArgs<T>& rhs) = default;
	inline IState<T>& state() const { return _state; }
	inline IState<T>* other() const { return _state; }
	inline OperatorArgs<T>& other(IState<T>* v) { _other = v; return *this; }
	inline bool intBased() const { return _intBased; }
	inline OperatorArgs<T>& intBased(bool v) { _intBased = v; return *this; }
	inline bool waiting() const { return _waiting; }
	inline OperatorArgs<T>* waiting(bool v) { _waiting = v; return *this; }
	inline size_t iterations() const { return _iterations; }
	inline OperatorArgs<T>& iterations(size_t v) { _iterations = v; return *this; }
	inline plas::var_t var() const { return _var; }
	inline OperatorArgs<T>* var(plas::var_t v) { _var = v; return *this; }
	inline plas::OctDiffConstraint<T> diffCons() const { return _diffCons; }
	inline OperatorArgs<T>& diffCons(plas::OctDiffConstraint<T> v) { _diffCons = v; return *this; }
	inline plas::OctConstraint<T> octCons() const { return _octCons; }
	inline OperatorArgs<T> octCons(plas::OctConstraint<T> v) { _octCons = v; return *this; }

private:
	IState<T>& _state;
	IState<T>* _other;
	bool _intBased;
	bool _waiting;
	size_t _iterations;
	plas::var_t _var;
	plas::OctDiffConstraint<T> _diffCons;
	plas::OctConstraint<T> _octCons;
};


// This is a POD-like object. Before use, call setup() with the appropriate
// arguments. Functions should throw exceptions whenever an error occurs, and
// set the isValid() flag to false if the State ends up being unusable for
// some reason (e.g. deallocated working memory without allocating another,
// etc). Non-initialized states are always invalid.
template <typename T> struct IState {
	virtual ~IState() {}
	virtual const IImplementation<T>& implementation() const = 0;
	virtual IState<T>* clone() const = 0;
	virtual bool isValid() const = 0;
	virtual size_t diffSize() const = 0;
	virtual size_t octSize() const { return diffSize() / 2; }
	virtual bool rowMajor() const = 0;
	virtual void copyTo(T* ptr) const = 0;
	virtual void setup(size_t diffSize, T* rawInput, bool rowMajor, void* implOptions = NULL) = 0;
};


// IOperatorResult must live until the operation is finished. It is a critical
// error to delete or collect its memory while the operation is still running,
// as the operator may execute in a different thread and may write results back
// once it is finished.
// Please, use OperatorResult<T> instead of dealing with IOperatorResult
// directly. It'll automatically wait for the operation to be finished before
// destroying the IOperatorResult when it is deleted/out-of-scope.
template <typename T> struct IOperatorResult {
	virtual ~IOperatorResult() {}
	virtual const IImplementation<T>& implementation() const = 0;
	virtual const IOperator<T>& oper() const = 0;
	virtual bool isFinished() const = 0;
	virtual bool boolResult() const = 0;
	virtual void wait() = 0;
};


// IOperator must be a singleton PER operation variant (operation + details).
// It must never contain any mutable internal state.
// IOperatorResult<T> may be of any class the IOperator deems needed to fulfill
// the operation execution. To get additional information not included in the
// IOperatorResult interface, a cast may be necessary, but care must be taken
// to ensure cast correctness (e.g. check operation and details to ensure from
// which class the operator comes from, or ensure the correct type at compile
// time by using the subclass directly).
template <typename T> struct IOperator {
	virtual ~IOperator() {}
	virtual const IImplementation<T>& implementation() const = 0;
	virtual ukoct::EOperation operation() const = 0;
	virtual ukoct::OperationDetails details() const = 0;
	virtual IOperatorResult<T>* run(const OperatorArgs<T>& args) const = 0;
};


// IImplementation must be a singleton PER implementation.
// It must never contain any mutable internal state.
template <typename T> struct IImplementation {
	virtual ~IImplementation() {}
	virtual bool isImplemented() const = 0;
	virtual EImplementation type() const = 0;
	virtual IState<T>* newState() const = 0;
	virtual const IOperator<T> * const* getOperators(size_t& numOperators) const = 0;

	virtual EElemType elemType() const { return ElemTypeInfo<T>::elemType(); }
	virtual size_t elemSize() const { return ElemTypeInfo<T>::elemSize(); }
	virtual T infinity() const { return ElemTypeInfo<T>::infinity(); }

	inline virtual const IOperator<T>* getOptimalOperator(EOperation oper) const {
		std::vector<const IOperator<T>*> result;
		filterOperators(result, 1, oper);
		assert(result.size() == 1, "Could not find optimal operator.");
		return result[0];
	}

	inline virtual void filterOperators(std::vector<const IOperator<T>*>& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false) const {
		size_t numOperators = 0;
		size_t operCount = 0;
		const IOperator<T>* const* ary = getOperators(numOperators);
		for (size_t i = 0; i < numOperators && operCount < maxOpers; ++i) {
			const IOperator<T>& op = *ary[i];
			bool match = details == 0 || (matchAnyDetails ? op.details().isAny(details) : op.details().isAll(details));
			if ( (oper == OPER_NONE || op.operation() == oper) && match ) {
				result.insert(result.end(), &op);
				operCount++;
			}
		}
	}

	inline virtual void getOptimalOperators(std::map<EOperation, const IOperator<T>*>& result) const {
		for (int oper = OPER_MIN_; oper <= OPER_MAX_; ++oper) {
			EOperation o = static_cast<EOperation>(oper);
			result[o] = getOptimalOperator(o);
		}
	}

	inline bool operator==(const IImplementation<T>& other) const {
		return this == &other ? true : type() == other.type() && elemType() == other.elemType();
	}

	inline bool operator!=(const IImplementation<T>& other) const {
		return !this->operator==(other);
	}
};


}


#endif /* UKOCT_CORE_INTERFACE_HPP_ */
