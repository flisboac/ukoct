#ifndef UKOCT_CORE_CHESHIRE_HPP_
#define UKOCT_CORE_CHESHIRE_HPP_

#include "plas.hpp"
#include "ukoct/core/defs.hpp"
#include "ukoct/core/interface.hpp"

namespace ukoct {

template <typename T> class State;
template <typename T> class Operator;
template <typename T> class Implementation;

template <typename T, template <typename IT> class I> class Cheshire {

	const IState<T>* self() const {
		return _self;
	}


	IState<T>* self() {
		return _self;
	}


	IState<T>* self(IState<T>* state) {
		_self = state;
		return _self;
	}


	void release() {
		delete _self;
		_self = NULL;
	}


	IState<T>* reset(IState<T>* state) {
		release();
		_self = state;
		return _self;
	}


protected:
	I<T>* _self;
};

/**
 * A PIMPL class for an IState object.
 *
 * This class expects to:
 * 1. Fully own its enclosed object; and
 * 2. Have its internal pointer not aliased anywhere (C99 restrict here?).
 *
 * There's no ownership transfer on any of the copying methods, and therefore,
 * it should be totally safe to use on STL containers. However, beware of the
 * copying times, as it may depend on various factors, such as problem size
 * and specific implementation details. In case this may turn to be a problem,
 * instead of relying on the default copying methods, the client should
 * instantiate and execute a copy operator, and then implement his own waiting
 * scheme (e.g. waiting for conclusion on another thread).
 */
template <typename T> class State : public IState<T>, public Cheshire<T, IState> {
public:
	typedef Cheshire<T, IState> SuperType;

	State() :
		SuperType::_self(NULL) {}


	explicit State(const State<T>& rhs) :
		SuperType::_self(rhs._self != NULL ? rhs._self->clone() : NULL) {}


	explicit State(const IState<T>& rhs) :
		SuperType::_self(rhs.clone()) {}


	explicit State(IState<T>* self) :
		SuperType::_self(self) {}


	State(const IImplementation<T>& impl) :
		SuperType::_self(impl.newState()) {}


	~State() {
		delete SuperType::_self;
	}


	State<T>& operator=(const State<T>& other) {
		if (&other != this)
			return this->operator=(*other._self);
		return *this;
	}


	State<T>& operator=(IState<T>& other) {
		if (&other != SuperType::_self) {
			IOperator<T>* op = implementation().newOperator(OPER_COPY);
			OperatorArgs<T> args(*SuperType::_self);
			args.waiting(true);
			args.other(other);
			op->run(args);
			op->wait();
			delete op;
		}
		return *this;
	}

	inline const IImplementation<T>& implementation() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->implementation(); }

	inline IState<T>* clone(const IState<T>* other = NULL) const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->clone(other); }

	inline bool isValid() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->isValid(); }

	inline size_t implSize() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->implSize(); }

	inline size_t diffSize() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->diffSize(); }

	inline size_t octSize() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->octSize(); }

	inline bool rowMajor() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->rowMajor(); }

	inline void copyTo(T* ptr) const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); SuperType::_self->copyTo(ptr); }

	inline void setup(size_t diffSize, T* rawInput, bool rowMajor, void* implOptions = NULL)
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); SuperType::_self->setup(diffSize, rawInput, rowMajor, implOptions); }

};


/**
 * A PIMPL class for an IOperator object.
 *
 * This class is more akin to an auto_ptr than the other two. Any copying
 * operation, be it by constructor or assignment, will transfer ownership
 * around.
 *
 * @see ukoct::IOperator<T>
 */
template <typename T> class Operator : public IOperator<T>, public Cheshire<T, IOperator> {
public:
	typedef Cheshire<T, IOperator> SuperType;

	Operator() :
		SuperType::_self(NULL) {}


	Operator(const Operator<T>& rhs) :
		SuperType::_self(rhs._self)
	{
		rhs._self = NULL;
	}


	Operator(IOperator<T>* rhs) :
		SuperType::_self(rhs) {}


	Operator(const IState<T>& state, EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) :
		SuperType::_self(NULL) { setup(state.implementation(), oper, details, matchAnyDetails); }


	Operator(const IImplementation<T>& impl, EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) :
		SuperType::_self(NULL) { setup(impl, oper, details, matchAnyDetails); }


	~Operator() {
		delete SuperType::_self;
	}


	inline ukoct::EOperation operation() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->operation(); }

	inline ukoct::OperationDetails details() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->details(); }

	inline void run(const OperatorArgs<T>& args) const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); SuperType::_self->run(args); }

	inline bool isFinished()
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->isFinished(); }

	inline void wait()
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); SuperType::_self->wait(); }

	inline bool boolResult() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->boolResult(); }

	inline const std::vector<Timing>& timings() const
		{ assert(SuperType::_self != NULL, "Missing internal pointer."); return SuperType::_self->timings(); }

private:
	void setup(const IImplementation<T>& impl, EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) {
		SuperType::_self = impl.newOperator(oper, details, matchAnyDetails);
		assert(SuperType::_self != NULL, "Operator is not present for the implementation.");
	}

};


/**
 * A PIMPL class for an IImplementation object.
 *
 * The behaviour of `delete` is dependent on how the Implementation object was
 * initialized:
 * 1. If initialized with a `const IImplementation<T>*`, the Implementation
 *    object won't "own" the pointer to IImplementation.
 * 2. If initialized with a `IImplementation<T>*`, the Implementation object
 *    will take ownership of the implementation it is wrapping, and at
 *    finalization, it will delete said pointer.
 *
 * Note that copying is not permitted, just as the original interface doesn't
 * provide means for that. However, it can be default-constructed. For that,
 * this class CAN NOT be used on STL containers.
 *
 * @see ukoct::IImplementation<T>
 */
template <typename T> class Implementation : public IImplementation<T>, public Cheshire<T, IImplementation> {
public:
	typedef Cheshire<T, IImplementation> SuperType;

	Implementation() :
		SuperType::_self(NULL),
		_trueSelf(NULL) {}


	Implementation(const IImplementation<T>* impl) :
		SuperType::_self(NULL),
		_trueSelf(impl) {}


	Implementation(IImplementation<T>* impl) :
		SuperType::_self(impl),
		_trueSelf(impl) {}


	~Implementation() {
		delete SuperType::_self;
	}


	inline EImplementation type() const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); return _trueSelf->type(); }

	inline bool intBased() const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); return _trueSelf->intBased(); }

	inline EElemType elemType() const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); return _trueSelf->elemType(); }

	inline T infinity() const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); return _trueSelf->infinity(); }

	inline IState<T>* newState() const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); return _trueSelf->newState(); }

	inline void getDetails(std::map<EOperation, std::vector<OperationDetails> >& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false) const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); _trueSelf->getDetails(result, maxOpers, oper, details, matchAnyDetails); }

	inline void filterOperators(std::vector<const IOperator<T>*>& result, size_t maxOpers = 0, EOperation oper = OPER_NONE, OperationDetails details = 0, bool matchAnyDetails = false) const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); _trueSelf->filterOperators(result, maxOpers, oper, details, matchAnyDetails); }

	inline IOperator<T>* newOperator(EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) const
		{ assert(_trueSelf != NULL, "Missing internal pointer."); return _trueSelf->newOperator(oper, details, matchAnyDetails); }

private:
	const IImplementation<T>* _trueSelf;
};


}


#endif /* UKOCT_CORE_CHESHIRE_HPP_ */
