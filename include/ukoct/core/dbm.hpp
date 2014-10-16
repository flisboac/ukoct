#ifndef UKOCT_CORE_DBM_HPP_
#define UKOCT_CORE_DBM_HPP_


#define ukoct_PREPAREBOOLOPER(op, operation, w) \
	{ \
		op = instantiate(operation); \
		OperatorArgs<T> args(*_self); \
		args.waiting(w); \
		args.intBased(intBased()); \
		op->run(args); \
	}

#define ukoct_EXECBOOLOPER(operation, ret) \
	{ \
		IOperator<T>* op; \
		ukoct_PREPAREBOOLOPER(op, operation, true); \
		op->wait(); \
		ret = op->boolResult(); \
		delete op; \
	}

namespace ukoct {

template <typename T> struct OctDbm {
public:

	OctDbm() :
		_self(NULL),
		_intBased(false),
		_variants() {}


	explicit OctDbm(const OctDbm<T>& other) :
		_variants(other._variants),
		_intBased(other._intBased)
	{
		_self = other._self != NULL ? other._self.clone() : NULL;
	}


	OctDbm(const IImplementation<T>& impl) : _intBased(false) {
		_self = impl.newState();
	}


	OctDbm(const IState<T>& other) : _intBased(false) {
		_self = other.clone();
	}


	OctDbm(IState<T>* other) :
		_self(other),
		_intBased(false),
		_variants() {}


	~OctDbm() {
		if (_self != NULL) delete _self;
	}


	inline bool intBased() const {
		return implementation().intBased() || _intBased;
	}


	inline OctDbm<T>& intBased(bool f) {
		_intBased = f;
		return *this;
	}


	inline bool isValid() const {
		return _self != NULL && _self->isValid();
	}


	inline size_t diffSize() const {
		return _self != NULL ? _self->diffSize() : 0;
	}


	inline size_t octSize() const {
		return _self != NULL ? _self->octSize() : 0;
	}


	inline T infinity() const {
		return _self != NULL ? _self->implementation().infinity() : T();
	}


	inline std::map<EOperation, OperationDetails>& variants() {
		return _variants;
	}


	inline const std::map<EOperation, OperationDetails>& variants() const {
		return _variants;
	}


	inline const IImplementation<T>* implementationPtr() const {
		return _self != NULL ? &(_self->implementation()) : NULL;
	}


	inline const IImplementation<T>& implementation() const {
		IImplementation<T>* impl = implementationPtr();
		ukoct::assert(impl != NULL, "Missing internal pointer.");
		return *impl;
	}


	IOperator<T>* opIsConsistent(bool waiting = true) {
		IOperator<T>* op = needsIntOperator() ? instantiate(OPER_ISINTCONSISTENT) : instantiate(OPER_ISCONSISTENT);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		op->run(args);
		return op;
	}


	bool isConsistent() {
		IOperator<T>* op = opIsConsistent(true);
		op->wait();
		bool ret = op;
		delete op;
		return ret;
	}


	IOperator<T>* opIsCoherent(bool waiting = true) {
		IOperator<T>* op;
		ukoct_PREPAREBOOLOPER(op, OPER_ISCOHERENT, waiting);
		return op;
	}


	bool isCoherent() {
		bool ret = false;
		ukoct_EXECBOOLOPER(OPER_ISCOHERENT, ret);
		return ret;
	}


	IOperator<T>* opIsClosed(bool waiting = true) {
		IOperator<T>* op;
		ukoct_PREPAREBOOLOPER(op, OPER_ISCLOSED, waiting);
		return op;
	}


	bool isClosed() {
		bool ret = false;
		ukoct_EXECBOOLOPER(OPER_ISCLOSED, ret);
		return ret;
	}


	IOperator<T>* opIsStronglyClosed(bool waiting = true) {
		IOperator<T>* op;
		ukoct_PREPAREBOOLOPER(op, OPER_ISSTRONGLYCLOSED, waiting);
		return op;
	}


	bool isStronglyClosed() {
		bool ret = false;
		ukoct_EXECBOOLOPER(OPER_ISSTRONGLYCLOSED, ret);
		return ret;
	}


	IOperator<T>* opIsTightlyClosed(bool waiting = true) {
		IOperator<T>* op;
		ukoct_PREPAREBOOLOPER(op, OPER_ISTIGHTLYCLOSED, waiting);
		return op;
	}


	bool isTightlyClosed() {
		bool ret = false;
		ukoct_EXECBOOLOPER(OPER_ISTIGHTLYCLOSED, ret);
		return ret;
	}


	IOperator<T>* opIsWeaklyClosed(bool waiting = true) {
		IOperator<T>* op;
		ukoct_PREPAREBOOLOPER(op, OPER_ISWEAKLYCLOSED, waiting);
		return op;
	}


	bool isWeaklyClosed() {
		bool ret = false;
		ukoct_EXECBOOLOPER(OPER_ISWEAKLYCLOSED, ret);
		return ret;
	}


	IOperator<T>* opIsTop(bool waiting = true) {
		IOperator<T>* op;
		ukoct_PREPAREBOOLOPER(op, OPER_ISTOP, waiting);
		return op;
	}


	bool isTop() {
		bool ret = false;
		ukoct_EXECBOOLOPER(OPER_ISTOP, ret);
		return ret;
	}


	// Unary Operators


	IOperator<T>* opClosure(bool waiting = true) {
		IOperator<T>* op = needsIntOperator() ? instantiate(OPER_TIGHTCLOSURE) : instantiate(OPER_CLOSURE);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		op->run(args);
		return op;
	}


	bool closure() {
		IOperator<T>* op = opClosure(true);
		op->wait();
		bool ret = op;
		delete op;
		return ret;
	}


	IOperator<T>* opShortestPath(size_t iters = 0, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_SHORTESTPATH);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.iterations(iters);
		op->run(args);
		return op;
	}


	OctDbm<T>& shortestPath(size_t iters = 0) {
		IOperator<T>* op = opShortestPath(iters, true);
		op->wait();
		delete op;
		return *this;
	}


	IOperator<T>* opStrengthen(bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_STRENGTHEN);
		OperatorArgs<T> args(*_self);
		args.waiting(true);
		args.intBased(intBased());
		op->run(args);
		return op;
	}


	OctDbm<T>& strengthen() {
		IOperator<T>* op = opStrengthen(true);
		op->wait();
		delete op;
		return *this;
	}


	IOperator<T>* opTighten(bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_TIGHTEN);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		op->run(args);
		return op;
	}


	OctDbm<T>& tighten() {
		IOperator<T>* op = opTighten(true);
		op->wait();
		delete op;
		return *this;
	}


	IOperator<T>* opTop(bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_TOP);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		op->run(args);
		return op;
	}


	OctDbm<T>& top() {
		IOperator<T>* op = opTop(true);
		op->wait();
		delete op;
		return *this;
	}


	// Binary Operators


	IOperator<T>* opPushDiffCons(plas::OctDiffConstraint<T>& f, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_PUSHDIFFCONS);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.diffCons(f);
		op->run(args);
		return op;
	}


	OctDbm<T>& operator<<(plas::OctDiffConstraint<T>& f) {
		IOperator<T>* op = opPushDiffCons(f, true);
		op->wait();
		delete op;
		return *this;
	}


	IOperator<T>* opPushOctCons(plas::OctConstraint<T>& f, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_PUSHOCTCONS);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.octCons(f);
		op->run(args);
		return op;
	}


	OctDbm<T>& operator<<(plas::OctConstraint<T>& f) {
		IOperator<T>* op = opPushOctCons(f, true);
		op->wait();
		delete op;
		return *this;
	}


	IOperator<T>* opForgetVar(plas::var_t v, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_FORGETOCTVAR);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.var(v);
		op->run(args);
		return op;
	}


	OctDbm<T>& operator>>(plas::var_t v) {
		IOperator<T>* op = opForgetVar(v, true);
		op->wait();
		delete op;
		return *this;
	}


	// Binary inter-matrix operators


	IOperator<T>* opCopy(const OctDbm<T>& other, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_COPY);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.other(other._self);
		op->run(args);
		return op;
	}


	OctDbm<T>& operator=(const OctDbm<T>& other) {
		IOperator<T>* op = opCopy(other, true);
		op->wait();
		bool ret = op;
		delete op;
		return ret;
	}


	IOperator<T>* opIncludes(const OctDbm<T>& other, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_INCLUDES);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.other(other._self);
		op->run(args);
		return op;
	}


	bool includes(const OctDbm<T>& other) {
		IOperator<T>* op = opIncludes(other, true);
		op->wait();
		bool ret = op;
		delete op;
		return ret;
	}


	IOperator<T>* opEquals(const OctDbm<T>& other, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_EQUALS);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.other(other._self);
		op->run(args);
		return op;
	}


	bool operator==(const OctDbm<T>& other) {
		IOperator<T>* op = opEquals(other, true);
		bool ret = op;
		delete op;
		return ret;
	}


	bool operator!=(const OctDbm<T>& other) {
		return !this->operator==(other);
	}


	IOperator<T>* opUnion(const OctDbm<T>& other, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_UNION);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.other(other._self);
		op->run(args);
		return op;
	}


	OctDbm<T>& operator&=(const OctDbm<T>& other) {
		IOperator<T>* op = instantiate(OPER_UNION);
		OperatorArgs<T> args(*_self);
		args.waiting(true);
		args.intBased(intBased());
		args.other(other._self);
		op->run(args);
		delete op;
		return *this;
	}


	IOperator<T>* opIntersection(const OctDbm<T>& other, bool waiting = true) {
		IOperator<T>* op = instantiate(OPER_INTERSECTION);
		OperatorArgs<T> args(*_self);
		args.waiting(waiting);
		args.intBased(intBased());
		args.other(other._self);
		op->run(args);
		return op;
	}


	OctDbm<T>& operator|=(const OctDbm<T>& other) {
		IOperator<T>* op = instantiate(OPER_INTERSECTION);
		OperatorArgs<T> args(*_self);
		args.waiting(true);
		args.intBased(intBased());
		args.other(other._self);
		op->run(args);
		delete op;
		return *this;
	}


	OctDbm<T> operator&(const OctDbm<T>& other) {
		OctDbm<T> result = *static_cast<OctDbm<T>*>(this);
		result &= other;
		return result;
	}


	OctDbm<T> operator|(const OctDbm<T>& other) {
		OctDbm<T> result = *static_cast<OctDbm<T>*>(this);
		result |= other;
		return result;
	}

private:
	inline bool needsIntOperator() const {
		return !implementation().intBased() && _intBased;
	}


	IOperator<T>* instantiate(EOperation operation) {
		IOperator<T>* op = implementation().newOperator(operation, _variants[operation]);
		ukoct::assert(op == NULL, "Operation not implemented.", ERR_NOTIMPL);
		return op;
	}

private:
	IState<T>* _self;
	std::map<EOperation, OperationDetails> _variants;
	bool _intBased;
};


}

#undef ukoct_PREPAREBOOLOPER
#undef ukoct_EXECBOOLOPER

#endif /* UKOCT_CORE_DBM_HPP_ */
