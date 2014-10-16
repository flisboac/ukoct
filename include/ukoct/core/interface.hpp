#ifndef UKOCT_CORE_INTERFACE_HPP_
#define UKOCT_CORE_INTERFACE_HPP_

#include "ukoct/core/defs.hpp"
#include "plas.hpp"


namespace ukoct {

template <typename T> struct IImplementation;
template <typename T> struct IState;
template <typename T> struct IOperator;

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
	OperatorArgs(IState<T>& state)
		: _state(state)
		, _other(NULL)
		, _intBased(false)
		, _waiting(true)
		, _iterations(0)
		, _var(0)
		, _diffCons()
		, _octCons()
		{}
	inline IState<T>& state() const { return _state; }
	inline IState<T>* other() const { return _other; }
	inline OperatorArgs<T>& other(IState<T>* v) { _other = v; return *this; }
	inline bool intBased() const { return _intBased; }
	inline OperatorArgs<T>& intBased(bool v) { _intBased = v; return *this; }
	inline bool waiting() const { return _waiting; }
	inline OperatorArgs<T>* waiting(bool v) { _waiting = v; return *this; }
	inline size_t iterations() const { return _iterations; }
	inline OperatorArgs<T>& iterations(size_t v) { _iterations = v; return *this; }
	inline plas::var_t var() const { return _var; }
	inline OperatorArgs<T>& var(plas::var_t v) { _var = v; return *this; }
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


/**
 * Interface to a POD-like object holding the DBM state.
 *
 * Before use, call setup() with the appropriate arguments. Functions should
 * throw exceptions whenever an error occurs, and set the isValid() flag to
 * false if the IState ends up being unusable for some reason (e.g.
 * deallocated working memory without allocating another, etc).
 * Non-initialized states are always invalid.
 *
 * The client code must ensure that the IImplementation instance used to
 * create IState will be "alive" (not deleted) for as long as there are
 * running operations on the IState.
 *
 * @see ukoct::IImplementation<T>
 */
template <typename T> struct IState {
	virtual ~IState() {}
	virtual const IImplementation<T>& implementation() const = 0;
	virtual IState<T>* clone() const = 0;
	virtual bool isValid() const = 0;
	virtual size_t implSize() const = 0;
	virtual size_t diffSize() const { return implSize(); }
	virtual size_t octSize() const { return diffSize() / 2; }
	virtual bool rowMajor() const = 0;
	virtual void copyTo(T* ptr) const = 0;
	virtual void setup(size_t diffSize, T* rawInput, bool rowMajor) = 0;
};


/**
 * Interface to the implementation of an DBM operation.
 *
 * IOperator does not imply neither synchronous nor asynchronous operation.
 * Unless explicitly stated by the implementation documentation, IOperator
 * instances should be considered potentially synchronous. In any case,
 * clients must ensure to wait() before trying to discard the IOperator or get
 * the operation results. Also, the client application must ensure that the
 * IOperator and the IState object are alive until the entire operation is
 * finished.
 *
 * IOperator is supposed to be non-copyable by clients, but nothing stops an
 * implementation to employ a copy scheme in its internal API, if needed.
 */
template <typename T> struct IOperator {
	virtual ~IOperator() {}
	virtual ukoct::EOperation operation() const = 0;
	virtual ukoct::OperationDetails details() const = 0;
	virtual void run(const OperatorArgs<T>& args) = 0;

	virtual bool isFinished() = 0;
	virtual void wait() = 0;
	virtual bool boolResult() const = 0;
	virtual const std::vector<Timing>& timings() const = 0;

	operator bool() {
		wait();
		return boolResult();
	}
};


/**
 * Interface providing dynamic access to `static const` data regarding an
 * implementation.
 *
 * ... Therefore, some points that should be considered:
 * 1. It SHOULD NOT contain any mutable internal state;
 * 2. IImplementation SHOULD be treated and manipulated as a
 *    `const IImplementation<T>*` objects; and
 * 3. It SHOULD provide at least one operator per operation variant
 *    (considering that a variant is a combination of operation and
 *    details).
 *
 * Note, though, that nothing prevents an implementation to be constructed
 * with special parameters or initial state. Also, IOperator instances
 * provided by the IImplementation may potentially come from different classes
 * in this specific case. However, compatibility must be assured for
 * operations on IStates created from IImplementations with the same `type()`
 * and `elemType()` (e.g. All OpenCL-float states must interoperate amongst
 * themselves).
 */
template <typename T> struct IImplementation {
	virtual ~IImplementation() {}
	virtual EImplementation type() const = 0;
	virtual bool intBased() const = 0;
	virtual EElemType elemType() const = 0;
	virtual T infinity() const = 0;
	virtual IState<T>* newState() const = 0;
	virtual void getDetails(std::map<EOperation, std::vector<OperationDetails> >& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false) const = 0;
	virtual void filterOperators(std::vector<IOperator<T>*>& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false) const = 0;

	virtual IOperator<T>* newOperator(EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) const {
		std::vector<IOperator<T>*> ops;
		filterOperators(ops, 1, oper, details, matchAnyDetails);
		return ops.back();
	}

	inline bool operator==(const IImplementation<T>& other) const {
		return this == &other ? true : type() == other.type() && elemType() == other.elemType();
	}

	inline bool operator!=(const IImplementation<T>& other) const {
		return !this->operator==(other);
	}
};

#if 0
template <typename T> static inline void filterOperators(IOperator<T>* const* ary, size_t numOperators, std::vector<IOperator<T>*>& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false, IOperator<T>* (*copyf)(IOperator<T>*)) const {
	size_t operCount = 0;
	for (size_t i = 0; i < numOperators && operCount < maxOpers; ++i) {
		IOperator<T>& op = *ary[i];
		bool match = details == 0 || (matchAnyDetails ? op.details().isAny(details) : op.details().isAll(details));
		if ( (oper == OPER_NONE || op.operation() == oper) && match ) {
			if (copyf != NULL) {
				result.insert(result.end(), copyf(&op));
			} else {
				result.insert(result.end(), &op);
			}
			operCount++;
		}
	}
}
#endif

template <typename T, ukoct::EImplementation I = IMPL_DEFAULT> struct ImplementationInfo {
	static constexpr bool valid = false;
	static constexpr EImplementation type = I;
	static constexpr bool intBased = ElemTypeInfo<T>::intBased;
	static constexpr EElemType elemType = ElemTypeInfo<T>::elemType;
	static constexpr size_t elemSize = ElemTypeInfo<T>::elemSize;
	static T infinity() { return ElemTypeInfo<T>::infinity(); }
	static T floor(T n) { return ElemTypeInfo<T>::floor(n); }
	static T mod(T n, T d) { return ElemTypeInfo<T>::mod(n, d); }
	static IImplementation<T>* newImplementation() { return NULL; }
	static const IImplementation<T>* implementation() { return NULL; }
};

}


#endif /* UKOCT_CORE_INTERFACE_HPP_ */
