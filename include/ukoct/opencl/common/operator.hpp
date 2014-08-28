/*
 * operator.hpp
 *
 *  Created on: 13 Aug 2014
 *      Author: flisboac
 */

#ifndef UKOCT_OPENCL_COMMON_OPERATOR_HPP_
#define UKOCT_OPENCL_COMMON_OPERATOR_HPP_


#include <vector>
#include <CL/cl.hpp>
#include "ukoct/core.hpp"

namespace ukoct {
namespace impl {
namespace opencl {

template <typename T> class State;

struct IEventCallback {
	virtual ~IEventCallback() {}
	virtual void eventCallback(cl_event event, cl_int status) = 0;
};


static void eventCallback(cl_event ev, cl_int status, void* data) {
	reinterpret_cast<IEventCallback*>(data)->eventCallback(ev, status);
}


static void setEventCallback(cl::Event event, IEventCallback& callback) {
	event.setCallback(CL_COMPLETE, eventCallback, &callback);
}


template <typename T> struct IOperator {
public:

	/**
	 * The arguments to an operator.
	 * This is a mere POD-like class used to specify arguments and options to
	 * an operator's execution. It'll also hold the operator's output, if any.
	 *
	 * It must be important to notice that IOperator will expect to receive an
	 * Args instance for its exclusive use, meaning that an Args instance must
	 * not be shared between different IOperator instances.
	 */
	class Args {
	public:

		Args() :
			_state(_selfState),
			_otherState(NULL),
			_intBased(false),
			_waiting(true),
			_iterations(0),
			_var(plas::INVALID_VAR) {}


		Args(State<T>& s) :
			_state(s),
			_otherState(NULL),
			_intBased(false),
			_waiting(true),
			_iterations(0),
			_var(plas::INVALID_VAR) {}


		Args(const Args& rhs) :
			_state(rhs._state),
			_otherState(rhs._otherState),
			_intBased(rhs._intBased),
			_waiting(rhs._waiting),
			_iterations(rhs._iterations),
			_var(rhs._var) {}

	inline State<T>& state() { return _state; }
	inline State<T>& state() const { return _state; }

	inline State<T>*& otherState() { return _otherState; }
	inline State<T>* otherState() const { return _otherState; }
	inline Args& otherState(State<T>& state) { _otherState = &state; return *this; }
	inline Args& otherState(State<T>* state) { _otherState = state; return *this; }

	inline const std::vector<cl::Event>& waitEvents() const { return _waitEvents; }
	inline std::vector<cl::Event>& waitEvents() { return _waitEvents; }
	inline Args& waitEvents(const std::vector<cl::Event>& other) { _waitEvents = other; return *this; }

	inline bool& intBased() { return _intBased; }
	inline const bool& intBased() const { return _intBased; }
	inline Args& intBased(bool flag) const { _intBased = flag; return *this; }

	inline bool& waiting() { return _waiting; }
	inline const bool& waiting() const { return _waiting; }
	inline Args& waiting(bool flag) const { _waiting = flag; return *this; }

	inline size_t& iterations() { return _iterations; }
	inline const size_t& iterations() const { return _iterations; }
	inline Args& iterations(size_t nr) { _iterations = nr; return *this; }

	inline plas::OctDiffConstraint<T>& octcons() { return _octcons; }
	inline const plas::OctDiffConstraint<T>& octcons() const { return _octcons; }
	inline Args& octcons(plas::OctDiffConstraint<T> other) { _octcons = other; return this; }

	inline plas::OctConstraint<T>& cons() { return _cons; }
	inline const plas::OctConstraint<T>& cons() const { return _cons; }
	inline Args& cons(plas::OctConstraint<T> other) { _cons = other; return this; }

	inline plas::var_t& var() { return _var; }
	inline const plas::var_t& var() const { return _var; }
	inline Args& var(plas::var_t v) { _var = v; return *this; }

#if 0
	inline bool& valid() { return _valid; }
	inline Args& valid(bool flag) { _valid = flag; return *this; }
	inline plas::IMatrix<T>* matResult() const { return _matResult; }
	inline Args& matResult(plas::IMatrix<T>& mat) { _valid = true; _matResult = &mat; return *this; }
	inline Args& matResult(plas::IMatrix<T>* mat) { _valid = true; _matResult = mat; return *this; }
	inline bool& boolResult() { return _boolResult; }
	inline Args& boolResult(bool flag) { _valid = true; _matResult = flag; return *this; }
	inline T& typeResult() { return _typeResult; }
	inline Args& typeResult(T res) { _typeResult = res; return *this; }
	inline Args& typeResults(const T& res) {_typeResult = res; return *this; }
	inline std::vector<cl::Event>& events() { return _events; }
	inline Args& events(std::vector<cl::Event>& events) { _events = events; return *this; }
	inline std::vector<cl::CommandQueue>& queues() { return _queues; }
	inline Args& queues(std::vector<cl::CommandQueue>& queues) { _queues = queues; return *this; }
#endif

	private:
		State<T>& _state;
		State<T>* _otherState;
		std::vector<cl::Event> _waitEvents;
		bool _intBased;
		bool _waiting;
		size_t _iterations;
		plas::OctDiffConstraint<T> _octcons;
		plas::OctConstraint<T> _cons;
		plas::var_t _var;

	private:
		State<T> _selfState;
	};


	class Result {
	public:
		Result() :
			_valid(false),
			_matResult(NULL),
			_boolResult(false),
			_typeResult(),
			_events(),
			_queues() {}


		Result(const Result& rhs) :
			_valid(rhs._valid),
			_matResult(rhs._matResult),
			_boolResult(rhs._boolResult),
			_typeResult(rhs._typeResult),
			_events(rhs._events),
			_queues(rhs._queues) {}


		bool valid() const { return _valid; }

		explicit operator bool&() { return _boolResult; }
		explicit operator bool&() const { return _boolResult; }
		Result& operator=(bool value) { _valid = true; _boolResult = value; return *this; }
		bool& boolean() { return _boolResult; }
		const bool& boolean() const { return _boolResult; }

		explicit operator plas::IMatrix<T>*&() { return _matResult; }
		explicit operator plas::IMatrix<T>*&() const { return _matResult; }
		Result& operator=(plas::IMatrix<T>* value) { _valid = true; _matResult = value; return *this; }
		plas::IMatrix<T>*& matrix() { return _matResult; }
		const plas::IMatrix<T>*& matrix() const { return _matResult; }

		explicit operator T&() { return _typeResult; }
		explicit operator T&() const { return _typeResult; }
		Result& operator=(T value) { _valid = true; _typeResult = value; return *this; }
		T& result() { return _typeResult; }
		const T& result() const { return _typeResult; }

		std::vector<cl::Event>& events() { return _events; }
		const std::vector<cl::Event>& events() const { return _events; }

		std::vector<cl::CommandQueue>& queues() { return _queues; }
		const std::vector<cl::CommandQueue>& queues() const { return _queues; }

	private:
		bool _valid;
		plas::IMatrix<T>* _matResult;
		bool _boolResult;
		T _typeResult;

		std::vector<cl::Event> _events;
		std::vector<cl::CommandQueue> _queues;
	};


	virtual ~IOperator() {}
	virtual IOperator<T>* instantiate() = 0;
	virtual ukoct::EOperation operation() = 0;
	virtual void run(const Args& args) = 0;
	virtual const Result& result() = 0;

	struct less {
		bool operator()(const IOperator<T>& a, const IOperator<T>& b) {
			return a.operation() < b.operation();
		}
	};
};


template <typename T> class Operator : public IOperator<T> {
public:
	Operator() : _operator(NULL) {}


	Operator(IOperator<T>* ptr) : _operator(ptr) {}


	Operator(const Operator<T>& other) : _operator(NULL) {
		if (other._operator != NULL) _operator = other._operator.copy();
	}


	~Operator() {
		if (_operator != NULL) delete _operator;
	}


	IOperator<T>* instantiate() {
		return new Operator(*this);
	}


	EOperation operation() {
		return _operator != NULL ? _operator->operation() : OPER_NONE;
	}


	IOperator<T>* wrappedOperator() {
		return _operator;
	}


	void run(const typename IOperator<T>::Args& args) {
		if (_operator != NULL)
			_operator->run(args);
		else
			throw Error("No operator implementation given to operator wrapper.");
	}


	const typename IOperator<T>::Result& result() {
		if (_operator != NULL)
			return _operator->result();
		else
			throw Error("No operator implementation given to operator wrapper.");
	}


private:
	IOperator<T>* _operator;
};


template <typename T> class MultiOperator : public IOperator<T> {
public:
	typedef typename IOperator<T>::Args Args;
	typedef typename IOperator<T>::Result Result;

	MultiOperator() : _oper(OPER_NONE) {}


	MultiOperator(EOperation operation) : _oper(operation) {}


	MultiOperator(EOperation operation, IOperator<T>* ptr) : _oper(operation) {
		_operators.push(Operator<T>(ptr));
	}


	MultiOperator(const Operator<T>& other) :
		_oper(other._oper),
		_operators(other._operators) {}


	IOperator<T>* instantiate() {
		return new MultiOperator(*this);
	}


	EOperation operation() {
		return _oper;
	}


	void run(const Args& args) {
		for (typename std::vector<Operator<T> >::iterator it = _operators.begin(); it != _operators.end(); ++it)
			it.run(args);
	}


	const Result& result() {
		if (_operators.size() > 0)
			return _operators.back()->result();
		else
			throw Error("No operator added to multi-operator container.");
	}


	const std::vector<Operator<T> >& operators() {
		return _operators;
	}


	void add(const Operator<T>& oper) {
		_operators.push_back(oper);
	}


	void add(IOperator<T>* oper) {
		_operators.push_back(Operator<T>(oper));
	}


private:
	EOperation _oper;
	std::vector<Operator<T> > _operators;
};


}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATOR_HPP_ */
