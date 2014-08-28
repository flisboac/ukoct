#ifndef UKOCT_OPENCL_DBM_HPP_
#define UKOCT_OPENCL_DBM_HPP_


#include <map>

namespace ukoct {
namespace impl {
namespace opencl {

template <> struct ImplData<cl_float> {
	static const bool specialized = true;
	static const cl_float hugeval() throw() { return std::numeric_limits<cl_float>::infinity(); }
	static const char* source() { return "" // to shut Eclipse up
		#include "ukoct/opencl/common/sources/defs.inc.cl"
		#include "ukoct/opencl/float/sources/defs.inc.cl"
		#include "ukoct/opencl/float/sources/consistent.inc.cl"
		#include "ukoct/opencl/float/sources/intconsistent.inc.cl"
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
#if 0
		impl::opencl::State<T>::OperatorIterator oper = _state->operators().find(OPER_CONSISTENT);
		if (oper == _state->operators().end())
			throw Error("No operator registered for 'consistent.'");
		impl::opencl::IOperator<T>::Args args;
		args.waiting = true;
		oper->second.run(_state, args);
		return args.boolResult;
#endif
		return false;
	}


	bool intConsistent(){
		// TODO
		return false;
	}


	bool coherent(){
		// TODO
		return false;
	}


	bool closed(){
		// TODO
		return false;
	}


	bool stronglyClosed(){
		// TODO
		return false;
	}


	bool tightlyClosed(){
		// TODO
		return false;
	}


	bool weaklyClosed(){
		// TODO
		return false;
	}


	bool isTop(){
		// TODO
		return false;
	}



	bool closure(){
		// TODO
		return false;
	}


	bool tightClosure(){
		// TODO
		return false;
	}


	DbmType& shortestPath(size_t iters = 0){
		// TODO
		return *this;
	}


	DbmType& strengthen(bool intBased = false){
		// TODO
		return *this;
	}


	DbmType& tighten(){
		// TODO
		return *this;
	}


	DbmType& top(){
		// TODO
		return *this;
	}


	DbmType& operator<<(plas::OctDiffConstraint<Type>& f){
		// TODO
		return *this;
	}


	DbmType& operator<<(plas::OctConstraint<Type>& f){
		// TODO
		return *this;
	}


	DbmType& operator>>(plas::var_t v){
		// TODO
		return *this;
	}


	bool includes(const DbmType& other){
		// TODO
		return false;
	}


	DbmType& operator=(const DbmType& rhs){
		// TODO
		return *this;
	}


	bool operator==(const DbmType& other){
		// TODO
		return false;
	}


	bool operator!=(const DbmType& other){
		// TODO
		return false;
	}


	bool operator<(const DbmType& other){
		// TODO
		return false;
	}


	DbmType operator&(const DbmType& other){
		// TODO
		return *this;
	}


	DbmType operator|(const DbmType& other){
		// TODO
		return *this;
	}


	DbmType& operator&=(const DbmType& other){
		// TODO
		return *this;
	}


	DbmType& operator|=(const DbmType& other){
		// TODO
		return *this;
	}


	/* -------- */


	bool setup(plas::IMatrix<Type>& inputMat) {
		// TODO
		return false;
	}


	bool setup(plas::IMatrix<Type>& inputMat, cl::Program& program) {
		// TODO
		return false;
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
