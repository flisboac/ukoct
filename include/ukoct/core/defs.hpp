#ifndef UKOCT_CORE_DEFS_HPP_
#define UKOCT_CORE_DEFS_HPP_


#include <string>
#include <exception>
#include "plas.hpp"

#define ukoct_STRQT(s) #s
#define ukoct_STRFY(s) ukoct_STRQT(s)

#define ukoct_NAME "uksat"
#define ukoct_BRIEF "A simple octagon-domain-based solver in CPU/GPU."
#define ukoct_AUTHORS "Flavio Lisboa <fl81@kent.ac.uk>"
#define ukoct_MAJORVERSION 0
#define ukoct_MINORVERSION 2
#define ukoct_PATCHVERSION 0
#define ukoct_RELEASETYPE  "beta"

#define ukoct_VERSIONNAME \
	ukoct_STRFY(ukoct_MAJORVERSION) \
    "." ukoct_STRFY(ukoct_MINORVERSION) \
    "." ukoct_STRFY(ukoct_PATCHVERSION) \
    "-" ukoct_RELEASETYPE
#define ukoct_RELEASENAME ukoct_NAME " " ukoct_VERSIONNAME

#ifndef ukoct_IMPL
#	define ukoct_IMPL IMPL_CPU
#endif

#ifdef ukoct_DEBUG
#	define ukoct_ASSERT(b, m) ukoct::assert(b, m)
#else
#	define ukoct_ASSERT(b, m)
#endif

namespace ukoct {

enum EImplementation {
	IMPL_NONE,
	IMPL_CPU,
	IMPL_OPENCL,

	IMPL_DEFAULT = ukoct_IMPL,
	IMPL_MIN_ = IMPL_CPU,
	IMPL_MAX_ = IMPL_OPENCL
};


enum EElemType {
	ELEM_NONE,
	ELEM_HALF,
	ELEM_FLOAT,
	ELEM_DOUBLE,
	ELEM_LDOUBLE,

	ELEM_DEFAULT = ELEM_FLOAT,
	ELEM_MIN_ = ELEM_HALF,
	ELEM_MAX_ = ELEM_LDOUBLE
};


/**
 * A simple enumeration of all possible operations on an octagon DBM.
 * It is believed (by the author, at least) that their names are descriptive
 * enough to dispense documentation.
 */
enum EOperation {
	OPER_NONE,              //!< No-op.
	OPER_COPY,              //!< Implements a simple matrix-to-matrix copy (copy assignment operation).
	OPER_ISCONSISTENT,      //!< @see IOctDbm<T>::consistent
	OPER_ISINTCONSISTENT,   //!< @see IOctDbm<T>::intConsistent
	OPER_ISCOHERENT,        //!< @see IOctDbm<T>::coherent
	OPER_ISCLOSED,          //!< @see IOctDbm<T>::closed
	OPER_ISSTRONGLYCLOSED,  //!< @see IOctDbm<T>::stronglyClosed
	OPER_ISTIGHTLYCLOSED,   //!< @see IOctDbm<T>::tightlyClosed
	OPER_ISWEAKLYCLOSED,    //!< @see IOctDbm<T>::weaklyClosed
	OPER_ISTOP,             //!< @see IOctDbm<T>::isTop

	OPER_CLOSURE,           //!< @see IOctDbm<T>::closure
	OPER_TIGHTCLOSURE,      //!< @see IOctDbm<T>::tightClosure
	OPER_SHORTESTPATH,      //!< @see IOctDbm<T>::shortestPath
	OPER_STRENGTHEN,        //!< @see IOctDbm<T>::strengthen
	OPER_TIGHTEN,           //!< @see IOctDbm<T>::tighten
	OPER_TOP,               //!< @see IOctDbm<T>::top

	OPER_PUSHDIFFCONS,      //!< @see IOctDbm<T>::operator<<(plas::OctDiffConstraint<T>)
	OPER_PUSHOCTCONS,       //!< @see IOctDbm<T>::operator<<(plas::OctConstraint<T>)
	OPER_FORGETOCTVAR,      //!< @see IOctDbm<T>::operator>(plas::var_t)

	OPER_EQUALS,            //!< @see OPctDbm<T>
	OPER_INCLUDES,          //!< @see IOctDbm<T>
	OPER_UNION,             //!< @see IOctDbm<T>::operator&
	OPER_INTERSECTION,      //!< @see IOctDbm<T>::operator|

	OPER_MIN_ = OPER_COPY,
	OPER_MAX_ = OPER_INTERSECTION
};


enum EOperDetails {
	O_NONE,

	// Memory type, mutually inclusive (max 3 options)
	O_MEM_BASE_ = 0,
	O_MEM_GLOBAL = 1 << O_MEM_BASE_,          // Alorithm executes operation on global memory (obviously excluding the input matrix that MUST be global...)
	O_MEM_LOCAL = 1 << (O_MEM_BASE_ + 1),     // Algorithm executes operation on local memory
	O_MEM_DUAL = O_MEM_GLOBAL | O_MEM_LOCAL,
	O_MEM_MIN_ = O_MEM_GLOBAL,
	O_MEM_MAX_ = O_MEM_DUAL,
	O_MEM = O_MEM_DUAL | O_MEM_GLOBAL | O_MEM_LOCAL,

	// Dimension options, mutually exclusive, non-ordered
	O_DIMS_BASE_ = O_MEM_BASE_ + 4,
	O_DIMS_OPTIMAL = 1 << O_DIMS_BASE_,   // Resizes the problem considering the underlying hardware, to improve performance
	O_DIMS_EXACT = 2 << O_DIMS_BASE_,     //
	O_DIMS_FULLGROUP = 3 << O_DIMS_BASE_, //
	O_DIMS_MIN_ = O_DIMS_OPTIMAL,
	O_DIMS_MAX_ = O_DIMS_FULLGROUP,
	O_DIMS = O_DIMS_OPTIMAL | O_DIMS_EXACT | O_DIMS_FULLGROUP,

	// Reduction options, mutually exclusive
	O_REDUCE_BASE_ = O_DIMS_BASE_ + 3,
	O_REDUCE_ELEM = 1 << O_REDUCE_BASE_,
	O_REDUCE_VEC = 2 << O_REDUCE_BASE_,
	O_REDUCE_MIN_ = O_REDUCE_ELEM,
	O_REDUCE_MAX_ = O_REDUCE_VEC,
	O_REDUCE = O_REDUCE_ELEM | O_REDUCE_VEC,

	// Execution options, mutually exclusive
	O_EXEC_BASE_ = O_REDUCE_BASE_ + 3,
	O_EXEC_ONEPASS = 1 << O_EXEC_BASE_,
	O_EXEC_LOOP = 2 << O_EXEC_BASE_,
	O_EXEC_QUEUED = 3 << O_EXEC_BASE_,
	O_EXEC_MIN_ = O_EXEC_ONEPASS,
	O_EXEC_MAX_ = O_EXEC_QUEUED,
	O_EXEC = O_EXEC_ONEPASS | O_EXEC_LOOP | O_EXEC_QUEUED,

	// Implementation Details, mutually inclusive
	O_IMPL_BASE_ = O_EXEC_BASE_ + 3,
	O_IMPL_INTERM = 1 << O_IMPL_BASE_,      // Implements an intermediate stage of an operation
	O_IMPL_PROXY = 1 << (O_IMPL_BASE_ + 1), // This implementation is a proxy, one or more operators may be internally executed in indeterminate order to implement the operation
	O_IMPL_MIN = O_IMPL_INTERM,
	O_IMPL_MAX = O_IMPL_PROXY,
	O_IMPL = O_IMPL_PROXY | O_IMPL_INTERM
};


class OperationDetails {
public:
	OperationDetails() : _flags(0) {};
	OperationDetails(const OperationDetails& other) = default;
	OperationDetails(unsigned int flags) : _flags(flags) {}
	inline operator const unsigned int&() const { return _flags; }
	inline operator unsigned int&() { return _flags; }
	inline OperationDetails mem() const { return _flags & O_MEM; }
	inline OperationDetails dims() const { return _flags & O_DIMS; }
	inline OperationDetails reduce() const { return _flags & O_REDUCE; }
	inline OperationDetails exec() const { return _flags & O_EXEC; }
	inline OperationDetails impl() const { return _flags & O_IMPL; }
	inline OperationDetails mem(OperationDetails oth) { return (_flags & ~O_MEM) | (oth & O_MEM); }
	inline OperationDetails dims(OperationDetails oth) { return (_flags & ~O_DIMS) | (oth & O_DIMS); }
	inline OperationDetails reduce(OperationDetails oth) { return (_flags & ~O_REDUCE) | (oth & O_REDUCE); }
	inline OperationDetails exec(OperationDetails oth) { return (_flags & ~O_EXEC) | (oth & O_EXEC); }
	inline OperationDetails impl(OperationDetails oth) { return (_flags & ~O_IMPL) | (oth & O_IMPL); }
	inline OperationDetails group(OperationDetails grp) {
		if (grp == O_MEM) return mem();
		if (grp == O_DIMS) return dims();
		if (grp == O_REDUCE) return reduce();
		if (grp == O_EXEC) return exec();
		if (grp == O_IMPL) return impl();
		return _flags;
	}
	inline OperationDetails group(OperationDetails grp, OperationDetails oth) {
		if (grp == O_MEM) return mem(oth);
		if (grp == O_DIMS) return dims(oth);
		if (grp == O_REDUCE) return reduce(oth);
		if (grp == O_EXEC) return exec(oth);
		if (grp == O_IMPL) return impl(oth);
		return _flags;
	}
	inline bool isMem(OperationDetails other, bool any = false) const {
		return any ? (mem() & other.mem() != 0) : (mem() & other.mem() == other.mem());
	}
	inline bool isDims(OperationDetails other) {
		return dims() == other.dims();
	}
	inline bool isReduce(OperationDetails other) {
		return reduce() == other.reduce();
	}
	inline bool isExec(OperationDetails other) {
		return exec() == other.exec();
	}
	inline bool isImpl(OperationDetails other, bool any = false) {
		return any ? (impl() & other.impl() != 0) : (impl() & other.impl() == other.impl());
	}
	inline bool isAny(OperationDetails other) {
		return isMem(other, true) || isDims(other) || isReduce(other) || isExec(other) || isImpl(other, true);
	}
	inline bool isAll(OperationDetails other) {
		return isMem(other, false) && isDims(other) && isReduce(other) && isExec(other) && isImpl(other, false);
	}

private:
	unsigned int _flags;
};


/**
 * The library's base exception class.
 */
class Error : public std::exception {
public:
	Error() throw() : _code(0) {}
	Error(int code) throw() : _code(code) {}
	Error(const char* what) throw() : _code(0), _what(what) {}
	Error(int code, const char* what) throw() : _code(code), _what(what) {}
	Error(const std::string& what) throw() : _code(0), _what(what) {}
	Error(int code, const std::string& what) throw() : _code(code), _what(what) {}
	virtual ~Error() throw() {}
	virtual const char* what() throw() { return _what.c_str(); }
	virtual int code() throw() { return _code; }

private:
	int _code;
	std::string _what;
};


/**
 * Exception thrown when a timeout happens.
 */
class TimeoutError : public Error {};


static inline void assert(bool expr, const char* msg) { if (!expr) throw Error(msg); }

template <typename T> static inline void assertStateOptions(bool initialized, size_t diffSize, T* rawInput, bool rowMajor) {
	assert(!initialized, "State cannot be initialized more than once.");
	assert(diffSize > 1, "diffSize must be at least 2.");
	assert(diffSize % 2 == 0, "diffSize must be a multiple of 2.");
	assert(rawInput != NULL, "Input not given.");
}


template <typename T> struct ElemTypeInfo {
	static const bool specialized = false;
	static constexpr T infinity() { return std::numeric_limits<T>::max(); }
	static constexpr EElemType elemType() { return ELEM_NONE; }
	static constexpr size_t elemSize() { return sizeof(T); }
};


template <> struct ElemTypeInfo<float> {
	static const bool specialized = true;
	static constexpr float infinity() { return std::numeric_limits<float>::infinity(); }
	static constexpr EElemType elemType() { return ELEM_FLOAT; }
	static constexpr size_t elemSize() { return sizeof(float); }
};


template <> struct ElemTypeInfo<double> {
	static const bool specialized = true;
	static constexpr double infinity() { return std::numeric_limits<float>::infinity(); }
	static constexpr EElemType elemType() { return ELEM_DOUBLE; }
	static constexpr size_t elemSize() { return sizeof(double); }
};


template <> struct ElemTypeInfo<long double> {
	static const bool specialized = true;
	static constexpr long double infinity() { return std::numeric_limits<long double>::infinity(); }
	static constexpr EElemType elemType() { return ELEM_LDOUBLE; }
	static constexpr size_t elemSize() { return sizeof(long double); }
};


}


#endif /* UKOCT_CORE_DEFS_HPP_ */
