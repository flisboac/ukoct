#ifndef UKOCT_CORE_DBM_HPP_
#define UKOCT_CORE_DBM_HPP_

namespace ukoct {

// TODO ukoct/core/dbm.hpp
#if 0
template <typename T> struct OctDbm;
template <typename T> class Promise;

template <typename T, typename R> class Promise {
public:

private:
	IOperator<T>* _oper;
};

template <typename T> struct OctDbm {
	virtual ~OctDbm() {}

	// Unary query operators
	virtual bool isValid()          const = 0;
	virtual size_t diffSize()       const = 0;       // number of variables in the difference-based octagonal system
	virtual size_t octSize()        const = 0;        // number of variables in the octagonal system
	virtual T infinity()            const = 0;            // Equivalent to infinity
	virtual bool isConsistent()           = 0;           // consistency
	virtual bool isConsistent()     const = 0;           // consistency
	virtual bool isIntConsistent()        = 0;        // ZConsistent
	virtual bool isIntConsistent()  const = 0;        // ZConsistent
	virtual bool isCoherent()             = 0;             // coherency
	virtual bool isCoherent()       const = 0;             // coherency
	virtual bool isClosed()               = 0;               // checks closedness
	virtual bool isClosed()         const = 0;               // checks closedness
	virtual bool isStronglyClosed()       = 0;       // checks strong closure
	virtual bool isStronglyClosed() const = 0;       // checks strong closure
	virtual bool isTightlyClosed()        = 0;        // checks tight closure
	virtual bool isTightlyClosed()  const = 0;        // checks tight closure
	virtual bool isWeaklyClosed()         = 0;         // checks weak closure
	virtual bool isWeaklyClosed()   const = 0;         // checks weak closure
	virtual bool isTop()                  = 0;                  // checks if dbm is the top
	virtual bool isTop()            const = 0;                  // checks if dbm is the top

	// Unary Operators
	virtual bool closure() = 0;                // shortest path closure + consistency check
	virtual bool closure() = 0;                // shortest path closure + consistency check
	virtual bool tightClosure() = 0;           // tight shortest path closure + z-consistency check
	virtual bool tightClosure() = 0;           // tight shortest path closure + z-consistency check
	virtual OctDbm<T>& shortestPath(size_t iters = 0) = 0;            // ...
	virtual OctDbm<T>& shortestPath(size_t iters = 0) = 0;            // ...
	virtual OctDbm<T>& strengthen(bool intBased = false) = 0;         // ...
	virtual OctDbm<T>& strengthen(bool intBased = false) = 0;         // ...
	virtual OctDbm<T>& tighten() = 0;                                 // ...
	virtual OctDbm<T>& tighten() = 0;                                 // ...
	virtual OctDbm<T>& top() = 0;                                     // Sets the matrix as top
	virtual OctDbm<T>& top() = 0;                                     // Sets the matrix as top

	// Binary Operators
	virtual OctDbm<T>& operator<<(plas::OctDiffConstraint<T>& f) = 0; // add an difference-based constraint
	virtual OctDbm<T>& operator<<(plas::OctConstraint<T>& f) = 0;     // add a difference constraint (will be split into two oct constraints)
	virtual OctDbm<T>& operator>>(plas::var_t v) = 0;                 // forgets a difference variable

	// Binary inter-matrix operators
	OctDbm<T>& operator=(const OctDbm<T>& rhs) = 0;    // copy assignment
	bool includes(const OctDbm<T>& other) = 0;        // inclusion
	bool operator==(const OctDbm<T>& other) = 0;       // equality
	bool operator!=(const OctDbm<T>& other) = 0;       // equality
	OctDbm<T> operator&(const OctDbm<T>& other) = 0;   // union / join
	OctDbm<T> operator|(const OctDbm<T>& other) = 0;   // intersection / meet
	OctDbm<T>& operator&=(const OctDbm<T>& other) = 0; // self-assigned union
	OctDbm<T>& operator|=(const OctDbm<T>& other) = 0; // self-assigned intersection
};
#endif

}

#endif /* UKOCT_CORE_DBM_HPP_ */
