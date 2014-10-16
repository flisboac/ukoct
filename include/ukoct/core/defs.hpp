#ifndef UKOCT_CORE_DEFS_HPP_
#define UKOCT_CORE_DEFS_HPP_


#include <string>
#include <cmath>
#include <exception>
#include <iomanip>
#include "plas.hpp"

#define ukoct_STRQT(s) #s
#define ukoct_STRFY(s) ukoct_STRQT(s)

#define ukoct_NAME "uksat"
#define ukoct_BRIEF "A simple octagon-domain-based solver in CPU/GPU."
#define ukoct_AUTHORS "Flavio Lisboa <flisboa.costa@gmail.com>"
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

enum EErr {
	OK = 0,
	ERROR,
	ERR_NOTIMPL
};

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
	OPER_ALL = -1,
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
	O_IMPL_INTERM = 1 << O_IMPL_BASE_,       // Implements an intermediate stage of an operation
	O_IMPL_PROXY = 1 << (O_IMPL_BASE_ + 1),  // This implementation is a proxy, one or more operators may be internally executed in indeterminate order to implement the operation
	O_IMPL_MUTATOR = 1 << (O_IMPL_BASE_ + 2), // This implementation is non-const, and may mutate the DBM state
	O_IMPL_MIN = O_IMPL_INTERM,
	O_IMPL_MAX = O_IMPL_MUTATOR,
	O_IMPL = O_IMPL_PROXY | O_IMPL_INTERM | O_IMPL_MUTATOR
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


enum ETimingResolution {
	TIME_NONE,
	TIME_NSEC, // nanosseconds
	TIME_USEC, // microsseconds
	TIME_MSEC, // milisseconds
	TIME_SEC,  // seconds
	TIME_MIN,  // minutes
	TIME_HOUR,  // hours

	TIME_MIN_ = TIME_NSEC,
	TIME_MAX_ = TIME_HOUR
};


class Timing;
class CpuTiming;


static inline void assert(bool expr, const char* msg, int code = ERROR) { if (!expr) throw Error(code, msg); }


template <typename T> static inline void assertStateOptions(bool initialized, size_t diffSize, T* rawInput, bool rowMajor) {
	assert(!initialized, "State cannot be initialized more than once.");
	assert(diffSize > 1, "diffSize must be at least 2.");
	assert(diffSize % 2 == 0, "diffSize must be a multiple of 2.");
	assert(rawInput != NULL, "Input not given.");
}


class Timing {
public:
	Timing() :
		_resolution(TIME_NONE),
		_time(),
		_stage(0),
		_stageName("") {}


	Timing(double t, ETimingResolution r, const char* n = NULL, int s = 0) :
		_resolution(r),
		_time(t),
		_stage(s),
		_stageName(n) {}


	Timing(const Timing& rhs) = default;


	inline ETimingResolution resolution() const {
		return _resolution;
	}


	inline Timing& resolution(ETimingResolution v) {
		_resolution = v;
		return *this;
	}


	inline double time() const {
		return _time;
	}


	inline double time(ETimingResolution res) {
		return convertTime(_time, _resolution, res);
	}


	inline Timing& time(double v) {
		_time = v;
		return *this;
	}


	inline Timing& time(double v, ETimingResolution r) {
		_time = v; _resolution = r;
		return *this;
	}


	inline int stage() const {
		return _stage;
	}


	inline Timing& stage(int v) {
		_stage = v;
		return *this;
	}


	const std::string& stageName() const {
		return _stageName;
	}


	Timing& stageName(const std::string& v) {
		_stageName = v;
		return *this;
	}


	Timing& stageName(const char* v) {
		_stageName = v;
		return *this;
	}


	Timing& round(unsigned int length = 3) {
		_time = roundTime(_time, _resolution, length);
		return *this;
	}


public:
	static inline const char* timeSuffix(ETimingResolution resolution) {
		switch(resolution) {
		case TIME_NSEC: return "ns"; break;
		case TIME_USEC: return "us"; break;
		case TIME_MSEC: return "ms"; break;
		case TIME_SEC : return "s" ; break;
		case TIME_MIN : return "m" ; break;
		case TIME_HOUR: return "h" ; break;
		}
		return "";
	}


	static inline double timeMult(ETimingResolution from, ETimingResolution to) {
		switch(from) {
		case TIME_NSEC:
			switch(to) {
			case TIME_NSEC: return 1; break;
			case TIME_USEC: return 1 / 1e3; break;
			case TIME_MSEC: return 1 / 1e6; break;
			case TIME_SEC : return 1 / 1e9; break;
			case TIME_MIN : return 1 / 1e9 * 60; break;
			case TIME_HOUR: return 1 / 1e9 * 60 * 60; break;
			}
			break;
		case TIME_USEC:
			switch(to) {
			case TIME_NSEC: return 1e3; break;
			case TIME_USEC: return 1; break;
			case TIME_MSEC: return 1 / 1e3; break;
			case TIME_SEC : return 1 / 1e6; break;
			case TIME_MIN : return 1 / 1e6 * 60; break;
			case TIME_HOUR: return 1 / 1e6 * 60 * 60; break;
			}
			break;
		case TIME_MSEC:
			switch(to) {
			case TIME_NSEC: return 1e6; break;
			case TIME_USEC: return 1e3; break;
			case TIME_MSEC: return 1; break;
			case TIME_SEC : return 1 / 1e3; break;
			case TIME_MIN : return 1 / 1e3 * 60; break;
			case TIME_HOUR: return 1 / 1e3 * 60 * 60; break;
			}
			break;
		case TIME_SEC :
			switch(to) {
			case TIME_NSEC: return 1e9; break;
			case TIME_USEC: return 1e6; break;
			case TIME_MSEC: return 1e3; break;
			case TIME_SEC : return 1; break;
			case TIME_MIN : return 1 / 60; break;
			case TIME_HOUR: return 1 / 60 * 60; break;
			}
			break;
		case TIME_MIN :
			switch(to) {
			case TIME_NSEC: return 60 * 1e9; break;
			case TIME_USEC: return 60 * 1e6; break;
			case TIME_MSEC: return 60 * 1e3; break;
			case TIME_SEC : return 60; break;
			case TIME_MIN : return 1; break;
			case TIME_HOUR: return 1 / 60; break;
			}
			break;
		case TIME_HOUR:
			switch(to) {
			case TIME_NSEC: return 60 * 60 * 1e9; break;
			case TIME_USEC: return 60 * 60 * 1e6; break;
			case TIME_MSEC: return 60 * 60 * 1e3; break;
			case TIME_SEC : return 60 * 60; break;
			case TIME_MIN : return 60; break;
			case TIME_HOUR: return 1; break;
			}
			break;
		}
		// Raise error?
		return 1;
	}


	static inline double convertTime(double time, ETimingResolution timeRes, ETimingResolution newRes) {
		if (time != 0 && timeRes != TIME_NONE && newRes != TIME_NONE && timeRes != newRes)
			time *= timeMult(timeRes, newRes);
		return time;
	}


	static inline double roundTime(double time, ETimingResolution& timeRes, unsigned int length = 3) {
		if (time != 0) {
			double factor = std::pow(10, length);
			int tres = timeRes;
			while (factor > time && tres < TIME_MIN_) {
				time /= factor;
				tres -= 1;
			}
			while (1 > time && tres > TIME_MAX_) {
				time *= factor;
				tres += 1;
			}
			timeRes = (ETimingResolution) tres;
		}
		return time;
	}

private:
	ETimingResolution _resolution; // For _time
	double _time;
	int _stage; // Implementation-dependent
	std::string _stageName; // Implementation-defined
};


struct CpuTiming : public Timing {
	clock_t clockStart;
	clock_t clockEnd;

	CpuTiming() : clockStart(0), clockEnd(0) {}
	CpuTiming(const CpuTiming& other) = default;


	inline void start() {
		clockStart = clock();
	}


	inline void end() {
		ukoct_ASSERT(isStarted(), "end() can only be called after start().");
		clockEnd = clock();
		time(totalSecs()).resolution(TIME_SEC).stageName("CPU Total");
	}


	inline bool isStarted() const {
		return clockStart != 0;
	}


	inline bool isFinished() const {
		return clockEnd != 0;
	}


	inline clock_t diff() const {
		ukoct_ASSERT(isFinished(), "diff() can only be called after end().");
		return clockEnd - clockStart;
	}


	inline clock_t rem() const {
		return diff() % CLOCKS_PER_SEC;
	}


	inline double totalSecs() const {
		ukoct_ASSERT(isStarted(), "end() can only be called after start().");
		return static_cast<double>(diff()) / static_cast<double>(CLOCKS_PER_SEC);
	}
};


inline void printTiming(double time, ETimingResolution resl, std::ostream& os, int precision = 3) {
	std::streamsize oldprec = os.precision();
	std::streamsize oldw = os.width();
	os << std::fixed << std::setw(0) << std::setprecision(precision) << time << Timing::timeSuffix(resl);
	os.unsetf(std::ios_base::floatfield);
	os.precision(oldprec);
	os.width(oldw);
}


inline void printTiming(const Timing& timing, std::ostream& os, int precision = 3) {
	printTiming(timing.time(), timing.resolution(), os, precision);
}


template <typename T> struct ElemTypeInfo {
	static constexpr bool specialized = false;
	static constexpr bool intBased = true;
	static constexpr EElemType elemType = ELEM_NONE;
	static constexpr size_t elemSize = sizeof(T);
	static inline T infinity() { return std::numeric_limits<T>::max(); }
	static inline T floor(T n) { return n; }
	static inline T mod(T n, T d) { return n % d; }
};


template <> struct ElemTypeInfo<float> {
	static constexpr bool specialized = true;
	static constexpr bool intBased = false;
	static constexpr EElemType elemType = ELEM_FLOAT;
	static constexpr size_t elemSize = sizeof(float);
	static inline float infinity() { return std::numeric_limits<float>::infinity(); }
	static inline float floor(float n) { return std::floor(n); }
	static inline float mod(float n, float d) { return std::fmod(n, d); }
};


template <> struct ElemTypeInfo<double> {
	static constexpr bool specialized = true;
	static constexpr bool intBased = false;
	static constexpr EElemType elemType = ELEM_DOUBLE;
	static constexpr size_t elemSize = sizeof(double);
	static inline double infinity() { return std::numeric_limits<double>::infinity(); }
	static inline double floor(double n) { return std::floor(n); }
	static inline double mod(double n, double d) { return std::fmod(n, d); }
};


template <> struct ElemTypeInfo<long double> {
	static constexpr bool specialized = true;
	static constexpr bool intBased = false;
	static constexpr EElemType elemType = ELEM_LDOUBLE;
	static constexpr size_t elemSize = sizeof(long double);
	static inline long double infinity() { return std::numeric_limits<long double>::infinity(); }
	static inline long double floor(long double n) { return std::floor(n); }
	static inline long double mod(long double n, long double d) { return std::fmod(n, d); }
};


}


#endif /* UKOCT_CORE_DEFS_HPP_ */
