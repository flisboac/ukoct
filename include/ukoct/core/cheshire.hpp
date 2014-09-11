#ifndef UKOCT_CORE_CHESHIRE_HPP_
#define UKOCT_CORE_CHESHIRE_HPP_

#include "plas.hpp"
#include "ukoct/core/defs.hpp"
#include "ukoct/core/interface.hpp"

namespace ukoct {

// TODO ukoct/core/cheshire.hpp
#if 0
template <typename T> class State;
template <typename T> class Operator;
template <typename T> class OperatorEnv;
template <typename T> class OperatorResult;


template <typename T> class State : public IState<T> {
public:
	State() : _self(NULL) {}
	explicit State(const State<T>& rhs) : _self(rhs._self != NULL ? rhs._self->clone() : NULL) {}
	explicit State(const IState<T>& rhs) : _self(rhs.clone()) {}
	explicit State(IState<T>* self) : _self(self) {}
	explicit State(const IImplementation<T>& impl) : _self(impl.newState()) {}
	~State() { if (_self != NULL) delete _self; }
	inline const IImplementation<T>& implementation() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->implementation(); }
	inline IState<T>* clone(const IState<T>* other = NULL) const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->clone(other); }
	inline bool isValid() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->isValid(); }
	inline size_t diffSize() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->diffSize(); }
	inline size_t octSize() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->octSize(); }
	inline bool rowMajor() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->rowMajor(); }
	inline T* rawInput() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->rawInput(); }
	inline void copyTo(T* ptr) const
		{ assert(_self != NULL, "Missing internal pointer."); _self->copyTo(ptr); }
	inline void setup(size_t diffSize, T* rawInput, bool rowMajor, void* implOptions = NULL)
		{ assert(_self != NULL, "Missing internal pointer."); _self->setup(diffSize, rawInput, rowMajor, implOptions); }

private:
	IState<T>* _self;
};


template <typename T> class OperatorArgs : public IOperatorArgs<T> {
public:
	OperatorArgs() : _self(NULL) {}
	explicit OperatorArgs(const OperatorArgs<T>& rhs) : _self(rhs._self != NULL ? rhs._self->clone() : NULL) {}
	explicit OperatorArgs(const IOperatorArgs<T>& rhs) : _self(rhs.clone()) {}
	explicit OperatorArgs(IOperatorArgs<T>* self) : _self(self) {}
	explicit OperatorArgs(const IState<T>& state) : _self(state.implementation().newOperatorArgs()) {}
	explicit OperatorArgs(const IOperator<T>& oper) : _self(oper.implementation().newOperatorArgs()) {}
	explicit OperatorArgs(const IImplementation<T>& impl) : _self(impl.newState()) {}
	~OperatorArgs() { if (_self != NULL) delete _self; }
	inline const IImplementation<T>& implementation() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->implementation(); }
	inline IOperatorArgs<T>* clone(const IOperatorArgs<T>* base = NULL) const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->clone(base); }
	inline IState<T>& state() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->state(); }
	inline IState<T>* other() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->other(); }
	inline IOperatorArgs<T>* other(IState* v)
		{ assert(_self != NULL, "Missing internal pointer."); return _self->other(v); }
	inline bool intBased() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->intBased(); }
	inline IOperatorArgs<T>* intBased(bool v)
		{ assert(_self != NULL, "Missing internal pointer."); return _self->intBased(v); }
	inline bool waiting() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->waiting(); }
	inline IOperatorArgs<T>* waiting(bool v)
		{ assert(_self != NULL, "Missing internal pointer."); return _self->waiting(v); }
	inline size_t iterations() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->iterations(); }
	inline IOperatorArgs<T>* waiting(size_t v)
		{ assert(_self != NULL, "Missing internal pointer."); return _self->iterations(v); }
	inline plas::var_t var() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->var(); }
	inline IOperatorArgs<T>* var(plas::var_t v)
		{ assert(_self != NULL, "Missing internal pointer."); return _self->var(v); }
	inline plas::OctDiffConstraint<T> diffCons() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->diffCons(); }
	inline IOperatorArgs<T>* diffCons(plas::OctDiffConstraint<T> v)
		{ assert(_self != NULL, "Missing internal pointer."); return _self->diffCons(v); }
	inline plas::OctConstraint<T> octCons() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->octCons(); }
	inline IOperatorArgs<T>* octCons(plas::OctConstraint<T> v)
		{ assert(_self != NULL, "Missing internal pointer."); return _self->octCons(v); }

private:
	IOperatorArgs<T>* _self;
};


template <typename T> class Operator : public IOperator<T> {
public:
	Operator() : _self(NULL) {}
	explicit Operator(const Operator<T>& rhs) : _self(rhs._self != NULL ? rhs._self->clone() : NULL) {}
	explicit Operator(const IOperator<T>& rhs) : _self(rhs.clone()) {}
	explicit Operator(IOperator<T>* self) : _self(self) {}
	explicit Operator(const IState<T>& state, EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) : _self(NULL) { setup(state.implementation().getOperatorEnv(), oper, details, matchAnyDetails); }
	explicit Operator(const IImplementation<T>& impl, EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) : _self(NULL) { setup(impl.getOperatorEnv(), oper, details, matchAnyDetails); }
	explicit Operator(const IOperatorEnv<T>& env, EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) : _self(NULL) { setup(env, oper, details, matchAnyDetails); }
	~Operator() { if (_self != NULL) delete _self; }
	inline const IImplementation<T>& implementation() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->implementation(); }
	inline IOperator<T>* instantiate() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->instantiate(); }
	inline ukoct::EOperation operation() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->operation(); }
	inline ukoct::OperationDetails details() const
		{ assert(_self != NULL, "Missing internal pointer."); return _self->details(); }
	inline void run(const IOperatorArgs<T>& args)
		{ assert(_self != NULL, "Missing internal pointer."); _self->run(args); }
	inline IOperatorResult<T>& result()
		{ assert(_self != NULL, "Missing internal pointer."); return _self->result(); }

private:
	void setup(const IOperatorEnv<T>& env, EOperation oper, OperationDetails details = 0, bool matchAnyDetails = false) {
		std::vector<IOperator<T>*> found;
		env.filter(found, 1, oper, details, matchAnyDetails);
		assert(!found.empty(), "Operator is not present for the implementation.");
		_self = found[0];
	}

private:
	IOperator<T>* _self;
};

#endif

}


#endif /* UKOCT_CORE_CHESHIRE_HPP_ */
