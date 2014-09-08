#ifndef UKOCT_OPENCL_DBM_HPP_
#define UKOCT_OPENCL_DBM_HPP_


#include <map>
#include "ukoct/opencl/common/operators.hpp"

namespace ukoct {
namespace impl {
namespace opencl {

template <> struct ImplData<cl_float> {
	static const bool specialized = true;
	static const cl_float hugeval() throw() { return std::numeric_limits<cl_float>::infinity(); }
	static constexpr const char* source() { return "" // to shut Eclipse up
		#include "ukoct/opencl/common/sources/defs.inc.cl"
		#include "ukoct/opencl/float/sources/defs.inc.cl"
		#include "ukoct/opencl/float/sources/addConstraint.inc.cl"
		#include "ukoct/opencl/float/sources/addDiffConstraint.inc.cl"
		#include "ukoct/opencl/float/sources/closed.inc.cl"
		#include "ukoct/opencl/float/sources/coherent.inc.cl"
		#include "ukoct/opencl/float/sources/consistent.inc.cl"
		#include "ukoct/opencl/float/sources/copy.inc.cl"
		#include "ukoct/opencl/float/sources/equals.inc.cl"
		#include "ukoct/opencl/float/sources/forget.inc.cl"
		#include "ukoct/opencl/float/sources/includes.inc.cl"
		#include "ukoct/opencl/float/sources/intConsistent.inc.cl"
		#include "ukoct/opencl/float/sources/intersection.inc.cl"
		#include "ukoct/opencl/float/sources/isTop.inc.cl"
		#include "ukoct/opencl/float/sources/shortestPath.inc.cl"
		#include "ukoct/opencl/float/sources/strengthen.inc.cl"
		#include "ukoct/opencl/float/sources/stronglyClosed.inc.cl"
		#include "ukoct/opencl/float/sources/tighten.inc.cl"
		#include "ukoct/opencl/float/sources/tightlyClosed.inc.cl"
		#include "ukoct/opencl/float/sources/top.inc.cl"
		#include "ukoct/opencl/float/sources/union.inc.cl"
		#include "ukoct/opencl/float/sources/weaklyClosed.inc.cl"
		;
	}
};

}
}


struct opencl;


/*
 * to
 * to, from
 * to, from, nvars
 * to, from, nvars, k
 * to, from, nvars, d
 * to, from, nvars, ca, cb, cd
 * to, from, nvars, caa, cab, cad, cba, cbb, cbd
 */

template <typename T> class OctDbm<T, ukoct::opencl> : public IOctDbm<T> {
public:
	typedef T Type;
	typedef ukoct::opencl Backend;
	typedef OctDbm<Type, ukoct::cpu> DbmType;
	typedef std::numeric_limits<Type> TypeLimits;
	typedef ukoct::impl::opencl::ImplData<Type> ImplData;


	OctDbm() : _state(new impl::opencl::State<T>()) {}
	OctDbm(const OctDbm<T, ukoct::opencl>& rhs) : _state(rhs._state->clone()) {}
	OctDbm(impl::opencl::State<T>* state) : _state(state) {}
	~OctDbm() { delete _state; }


	bool valid(){
		return ImplData::specialized && _state->valid();
	}


	size_t size(){
		return _state->size();
	}


	size_t octSize(){
		return _state->octSize();
	}


	T hugeval(){
		return _state->hugeval();
	}


	bool consistent(){
		impl::opencl::octdiff::ConsistentOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}


	bool intConsistent(){
		impl::opencl::octdiff::IntConsistentOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}


	bool coherent(){
		impl::opencl::octdiff::CoherentOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}


	bool closed(){
		impl::opencl::octdiff::ClosedOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}


	bool stronglyClosed(){
		impl::opencl::octdiff::StronglyClosedOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}


	bool tightlyClosed(){
		impl::opencl::octdiff::TightlyClosedOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}


	bool weaklyClosed(){
		impl::opencl::octdiff::WeaklyClosedOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}


	bool isTop(){
		impl::opencl::octdiff::IsTopOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return oper.result().boolean();
	}



	bool closure(){
		shortestPath();
		if (!consistent())
			return false;
		//strengthen();
		return true;
	}


	bool tightClosure(){
		shortestPath();
		if (!consistent())
			return false;
		tighten();
		if (!intConsistent())
			return false;
		return true;
	}


	DbmType& shortestPath(size_t iters = 0){
		impl::opencl::octdiff::ShortestPathOperator_GlobalLoop<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.iterations(iters == 0 ? _state->size() : iters);
		oper.run(args);
		return *this;
	}


	DbmType& strengthen(bool intBased = false){
		impl::opencl::octdiff::StrengthenOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.intBased(intBased);
		oper.run(args);
		return *this;
	}


	DbmType& tighten(){
		impl::opencl::octdiff::TightenOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return *this;
	}


	DbmType& top(){
		impl::opencl::octdiff::TopOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		oper.run(args);
		return *this;
	}


	DbmType& operator<<(plas::OctDiffConstraint<Type>& f){
		impl::opencl::octdiff::AddDiffConstraintOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.diffcons(f);
		oper.run(args);
		return *this;
	}


	DbmType& operator<<(plas::OctConstraint<Type>& f){
		impl::opencl::octdiff::AddConstraintOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.cons(f);
		oper.run(args);
		return *this;
	}


	DbmType& operator>>(plas::var_t v){
		impl::opencl::octdiff::ForgetOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.var(v);
		oper.run(args);
		return *this;
	}


	bool includes(const DbmType& other){
		if (other.size() != size())
			return false;
		impl::opencl::octdiff::IncludesOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.otherState(other._state);
		oper.run(args);
		return oper.result().boolean();
	}


	DbmType& operator=(const DbmType& other){
		if (!_state.valid())
			setup(other._state.size(), other._state.rowMajor(), NULL, 0, other._state.program());
		if (other.size() != size())
			return *this;
		impl::opencl::octdiff::CopyOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.otherState(other._state);
		oper.run(args);
		return *this;
	}


	bool operator==(const DbmType& other){
		if (other.size() != size())
			return false;
		impl::opencl::octdiff::IncludesOperator_GlobalReduce<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.otherState(other._state);
		oper.run(args);
		return oper.result().boolean();
	}


	bool operator!=(const DbmType& other){
		return !this->operator==(other);
	}


	DbmType operator&(const DbmType& other){
		if (other.size() != size())
			return *this;
		impl::opencl::octdiff::UnionOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.otherState(other._state);
		oper.run(args);
		return oper.result().boolean();
	}


	DbmType operator|(const DbmType& other){
		DbmType dbm;
		dbm.setup(other._state.size(), other._state.rowMajor(), NULL, 0, other._state.program());
		impl::opencl::octdiff::CopyOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.otherState(other._state);
		oper.run(args);
		dbm |= other;
		return dbm;
	}


	DbmType& operator&=(const DbmType& other){
		DbmType dbm;
		dbm.setup(other._state.size(), other._state.rowMajor(), NULL, 0, other._state.program());
		impl::opencl::octdiff::CopyOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.otherState(other._state);
		oper.run(args);
		dbm &= other;
		return dbm;
	}


	DbmType& operator|=(const DbmType& other){
		if (other.size() != size())
			return *this;
		impl::opencl::octdiff::IntersectionOperator_Global<T> op_;
		impl::opencl::IOperator<T>& oper = op_;
		typename impl::opencl::IOperator<T>::Args args(*_state);
		args.waiting(true);
		args.otherState(other._state);
		oper.run(args);
		return *this;
	}


	/* -------- */


	void setup(size_t diffSize, bool rowMajor = true, T* data = NULL, cl_mem_flags flags = 0, cl::Program* program = NULL) {
		cl::Program prg;
		if (program == NULL) {
			cl::Context context = impl::opencl::createContext(1);
			prg = impl::opencl::createProgramFromSource(context, impl::opencl::ImplData<T>::source());
			program = &prg;
		}
		setup(*program, diffSize, rowMajor, data, flags);
	}


	void setup(cl::Program program, size_t diffSize, bool rowMajor = true, T* data = NULL, cl_mem_flags flags = 0) {
		_state->setup(program, diffSize, rowMajor, data, flags);
	}


	cl::Program& clProgram() {
		return _state->program();
	}


private:
	impl::opencl::State<T>* _state;
};


template <typename T> class ClOctDbm : public OctDbm<T, ukoct::opencl> {};

}

#endif /* UKOCT_OPENCL_DBM_HPP_ */
