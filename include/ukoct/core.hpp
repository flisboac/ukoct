#ifndef UKOCT_CORE_HPP_
#define UKOCT_CORE_HPP_

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

namespace ukoct {


/**
 * A simple enumeration of all possible operations on an octagon DBM.
 * It is believed (by the author, at least) that their names are descriptive
 * enough to dispense documentation.
 */
enum EOperation {
	OPER_INTERMEDIATE = -1, //!< An intermediate or incomplete operation. Must be the identifier for all partial operations.
	OPER_NONE,              //!< No-op.
	OPER_CONSISTENT,        //!< @see IOctDbm<T>::consistent
	OPER_INTCONSISTENT,     //!< @see IOctDbm<T>::intConsistent
	OPER_COHERENT,          //!< @see IOctDbm<T>::coherent
	OPER_CLOSED,            //!< @see IOctDbm<T>::closed
	OPER_STRONGLYCLOSED,    //!< @see IOctDbm<T>::stronglyClosed
	OPER_TIGHTLYCLOSED,     //!< @see IOctDbm<T>::tightlyClosed
	OPER_WEAKLYCLOSED,      //!< @see IOctDbm<T>::weaklyClosed
	OPER_ISTOP,             //!< @see IOctDbm<T>::isTop
	OPER_CLOSURE,           //!< @see IOctDbm<T>::closure
	OPER_TIGHTCLOSURE,      //!< @see IOctDbm<T>::tightClosure
	OPER_SHORTESTPATH,      //!< @see IOctDbm<T>::shortestPath
	OPER_STRENGTHEN,        //!< @see IOctDbm<T>::strengthen
	OPER_TIGHTEN,           //!< @see IOctDbm<T>::tighten
	OPER_TOP,               //!< @see IOctDbm<T>::top
	OPER_ADDCONS,           //!< @see IOctDbm<T>::operator<<(plas::OctDiffConstraint<T>)
	OPER_ADDOCTCONS,        //!< @see IOctDbm<T>::operator<<(plas::OctConstraint<T>)
	OPER_FORGET,            //!< @see IOctDbm<T>::operator>(plas::var_t)
	OPER_COMPARE,           //!< @see IOctDbm<T>::compare
	OPER_UNION,             //!< @see IOctDbm<T>::operator&
	OPER_INTERSECTION       //!< @see IOctDbm<T>::operator|
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


/**
 * A generalized interface for an octagon DBM.
 *
 * The intention is that methods declared here are ensured to be implemented by
 * subclasses, so that clients can be safely use this basic interface to
 * create or extend a solving algorithm regardless of the DBM's implementation.
 *
 * It is assumed that implementations of the DBM keep coherency, given an
 * initially-coherent input matrix. Optionally, users can check coherency of an
 * input matrix through `coherent()`. It can be assumed that, as long as the
 * DBM is `valid()`, the DBM is also coherent.
 *
 * Implementations are free to throw exceptions whenever needed (e.g. an
 * internal or non-recoverable error occurred), and the client must be aware
 * of this possibility. Guarantees about validity of a DBM after an exception
 * is thrown during an operation must be given explicitly by the implementation
 * documentation, and in that event the implementation must also set the
 * DBM's `valid` flag accordingly. Optionally, the user can call `valid()`
 * to confirm if it is possible to keep using the DBM for further operations.
 *
 * The interface also softly requires that subclasses implement a set of
 * operations that may depend on the underlying implementation. They are not
 * declared directly on the interface because their signatures or semantics
 * may depend on the knowledge of implementation details. Those are:
 * - `int compare(Dbm<T> other)`: Returns a result equivalent to the default
 *    `strcmp` in semantics. That is, if the return value is less than zero
 *    (at least one variable in `this` compares less than its counterpart in
 *    `other`), `this` is a problem subset of `other`; if the return value
 *    is greater than zero (at least one value in `this` compares greater than
 *    its counterpart in `other`), the compared octagons represent different
 *    solutions; else, if the value equals zero, all values are equal and
 *    the octagons represent the same solution.
 * - `operator==`, `operator!=` and `operator<`, following the semantics of
 *   `compare`.
 * - `operator=`: the assignment operator.
 * - `operator|=` and `operator|`: Intersection / octagon-meet.
 * - `operator&=` and `operator&`: Union / octagon-join.
 *
 * Implementations are not enforced to be copy-constructible, but they must
 * provide (may be too obvious but bear with it for a moment, here we go!) a
 * default constructor.
 */
template <typename T> struct IOctDbm {
	virtual ~IOctDbm() {}

	/**
	 * Checks whether the current internal state of the DBM is valid.
	 * This method should be used to check whether the DBM can be used for
	 * further operations. If for any reason the DBM could or was not
	 * initialized, or was corrupted during an operation, this flag should be
	 * set.
	 *
	 * @return `true` if the DBM is in a valid state, *disregarding coherency*,
	 * and `false` otherwise.
	 */
	virtual bool valid() = 0;

	/**
	 * Returns the size of the problem, in terms of the difference-based
	 * matrix. This value equals `octSize() * 2`.
	 * @return The size of the problem in the difference-bound representation.
	 */
	virtual size_t size() = 0;                 // number of variables in the difference-based octagonal system

	/**
	 * The number of variables in the original problem in the octagon domain.
	 * @return The size of the problem in the octagon domain.
	 */
	virtual size_t octSize() = 0;              // number of variables in the octagonal system

	/**
	 * The value used to represent the top value in a specific implementation,
	 * generally infinity.
	 * @return The top value.
	 */
	virtual T hugeval() = 0;                   // Equivalent to infinity

	/**
	 * Checks if the DBM is consistent.
	 * @return `true` if the DBM is consistent, `false` otherwise.
	 */
	virtual bool consistent() = 0;             // consistency

	/**
	 * Checks if the DBM is z-consistent.
	 * @return `true` if the DBM is z-consistent, `false` otherwise.
	 */
	virtual bool intConsistent() = 0;          // ZConsistent

	/**
	 * Checks if the DBM is coherent.
	 * This method is only useful for initial runs of the DBM, to check if a
	 * given input DBM is coherent. Consequent operations on the matrix are
	 * supposed to keep coherency as long as the DBM itself keeps being valid.
	 * Another main use-case for this method is on unit testing, to see if a
	 * given operation makes the DBM non-coherent.
	 * @return `true` if the current DBM is coherent, `false` otherwise.
	 */
	virtual bool coherent() = 0;               // coherency

	/**
	 * Checks whether the DBM is closed.
	 * That is, `forall k, x, y, m(ij) = m(ik) + m(kj)`
	 * @return `true` if the current DBM is closed, `false` otherwise.
	 */
	virtual bool closed() = 0;                 // checks closedness

	/**
	 * Checks whether the DBM is strongly closed.
	 * @return `true` if the current DBM is strongly closed, `false` otherwise.
	 */
	virtual bool stronglyClosed() = 0;         // checks strong closure

	/**
	 * Checks whether the DBM is tightly closed.
	 * This is the integer version of closure checking.
	 * @return `true` if the current DBM is tightly closed, `false` otherwise.
	 */
	virtual bool tightlyClosed() = 0;          // checks tight closure

	/**
	 * Checks whether the DBM is weakly closed.
	 * @return `true` if the current DBM is weakly closed, `false` otherwise.
	 */
	virtual bool weaklyClosed() = 0;           // checks weak closure

	/**
	 * Checks if this DBM is the top DBM.
	 * @return `true` if the current DBM is top, `false` otherwise.
	 */
	virtual bool isTop() = 0;                  // checks if dbm is the top

	virtual bool closure() = 0;                // shortest path closure + consistency check
	virtual bool tightClosure() = 0;           // tight shortest path closure + z-consistency check
	virtual IOctDbm<T>& shortestPath(size_t iters = 0) = 0;            // ...
	virtual IOctDbm<T>& strengthen(bool intBased = false) = 0;         // ...
	virtual IOctDbm<T>& tighten() = 0;                                 // ...
	virtual IOctDbm<T>& top() = 0;                                     // Sets the matrix as top
	virtual IOctDbm<T>& operator<<(plas::OctDiffConstraint<T>& f) = 0; // add an difference-based constraint
	virtual IOctDbm<T>& operator<<(plas::OctConstraint<T>& f) = 0;     // add a difference constraint (will be split into two oct constraints)
	virtual IOctDbm<T>& operator>>(plas::var_t v) = 0;                 // forgets a difference variable
};


/**
 * @class OctDbm
 *
 * The implementation of a
 */
#ifdef ukoct_DEFAULT_OPENCL
struct opencl;
template <typename T, class Impl = opencl> class OctDbm : public IOctDbm<T> {
#else
struct cpu;
template <typename T, class Impl = cpu> class OctDbm : public IOctDbm<T> {
#endif

public:
	typedef T Type;
	typedef std::numeric_limits<T> TypeLimits;

	// Default constructor
	// Copy constructor
	//bool includes(const OctDbm<T, Impl>& other) = 0;               // inclusion
	//OctDbm<T, Impl>& operator=(const OctDbm<T, Impl>& rhs) = 0;    // assignment
	//bool operator==(const OctDbm<T, Impl>& other) = 0;             // equality
	//bool operator!=(const OctDbm<T, Impl>& other) = 0;             // equality
	//bool operator<(const OctDbm<T, Impl>& other) = 0;              // subset check
	//OctDbm<T, Impl> operator&(const OctDbm<T, Impl>& other) = 0;   // union / join
	//OctDbm<T, Impl> operator|(const OctDbm<T, Impl>& other) = 0;   // intersection / meet
	//OctDbm<T, Impl>& operator&=(const OctDbm<T, Impl>& other) = 0; // self-assigned union
	//OctDbm<T, Impl>& operator|=(const OctDbm<T, Impl>& other) = 0; // self-assigned intersection
};

};



#endif /* UKOCT_CORE_HPP_ */
