#ifndef UKOCT_UTIL_LOCK_HPP_
#define UKOCT_UTIL_LOCK_HPP_

#if !defined(ukoct_WIN32LOCK) && ( defined(_WIN32) || defined(_WIN64) )
#	define ukoct_WIN32LOCK
#endif

#if !defined(ukoct_PTHREADLOCK) && \
	(\
		defined(__unix__) || defined(__unix) || \
        (defined(__APPLE__) && defined(__MACH__)) \
	)
#	define ukoct_PTHREADLOCK
#endif

#ifdef ukoct_PTHREADLOCK
#	include <pthread.h>
#endif

namespace ukoct {
namespace util {

struct ILock {
	virtual ~ILock() {}
	virtual void read() = 0;
	virtual bool tryRead() = 0;
	virtual void unlockRead() = 0;

	virtual void write() = 0;
	virtual bool tryWrite() = 0;
	virtual void unlockWrite() = 0;
};


class NullLock {
	~NullLock() {}
	void read() {};
	bool tryRead() { return true; };
	void unlockRead() {};

	void write() {};
	bool tryWrite() { return true; };
	void unlockWrite() {};
};


#if defined(ukoct_WIN32LOCK)
class Win32Lock : public ILock {

};

class NativeLock : public Win32Lock {};

#elif defined(ukoct_PTHREADLOCK)
class PthreadLock : public ILock {

};

class NativeLock : public NullLock {};
#else
class NativeLock : public NullLock {}
#endif

}
}

#endif /* UKOCT_UTIL_LOCK_HPP_ */
