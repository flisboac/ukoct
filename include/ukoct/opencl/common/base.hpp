#ifndef UKOCT_OPENCL_BASE_HPP_
#define UKOCT_OPENCL_BASE_HPP_


#include <cmath>
#include <limits>
#include <vector>
#include <list>
#include <map>
#include <CL/cl.hpp>
#include "ukoct/core.hpp"
#include "ukoct/opencl/common/pool.hpp"
#include "ukoct/opencl/common/operator.hpp"


namespace ukoct {
namespace impl {
namespace opencl {


/**
 * Holds implementation information about the OpenCL DBM's format.
 */
template <typename T> struct ImplData {
	/**
	 * Specialization flag: if true, the implementation was specialized for
	 */
	static const bool specialized = false;

	/**
	 * Returns the "huge" value for the type. For floating-point types, "huge" is
	 * infinity, or the maximum value (as per the numeric limits).
	 */
	static const T hugeval() throw() { return std::numeric_limits<cl_float>::max(); }

	/**
	 * Holds the OpenCL's code for the implementation. May be an empty string if not
	 * applicable for a particular type.
	 */
	static const char* source() { return ""; }
};


/**
 * Comparison operator class for `cl::Device`.
 */
struct Device_less {
	bool operator()(const cl::Device& a, const cl::Device& b) { return a() < b(); }
};


template <typename T> class BufferKey {
public:
	static const cl_mem_flags FlagsMask = ~(CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_USE_HOST_PTR);

	BufferKey() :
		_nelems(0),
		_flags(CL_MEM_READ_WRITE) {}


	BufferKey(const BufferKey& rhs) :
		_nelems(rhs._nelems),
		_flags(rhs._flags) {}


	explicit BufferKey(const cl::Buffer& buffer) {
		*this = buffer;
	}


	BufferKey(size_t nelems, cl_mem_flags flags) :
		_nelems(nelems),
		_flags(flags & FlagsMask) {}


	BufferKey<T>& operator=(const cl::Buffer& buffer) {
		_nelems = buffer.getInfo<CL_MEM_SIZE>() / sizeof(T);
		_flags = buffer.getInfo<CL_MEM_FLAGS>() & FlagsMask;
		return *this;
	}


	bool operator<(const BufferKey& other) const { return compare(other) < 0; }
	bool operator==(const BufferKey& other) const { return compare(other) == 0; }
	bool operator!=(const BufferKey& other) const { return compare(other) != 0; }

	int compare(const BufferKey<T>& other) {
		int nelems_cmp = (_nelems > other._nelems) - (_nelems < other._nelems);
		int flags_cmp = (_flags > other._flags) - (_flags < other._flags);
		return nelems_cmp == 0 ? flags_cmp : nelems_cmp;
	}


	size_t size() const { return sizeof(T) * _nelems; }
	size_t nelems() const { return _nelems; }
	cl_mem_flags flags() const { return _flags; }

private:
	size_t _nelems;
	cl_mem_flags _flags;
};


template <typename T> class BufferPool : public MultiPool<cl::Buffer, BufferKey<T> > {
public:
	typedef T ElemType;
	typedef cl::Buffer Type;
	typedef MultiPool<cl::Buffer, BufferKey<T> > SuperType;
	typedef typename SuperType::KeyIterator KeyIterator;
	typedef typename SuperType::ConstKeyIterator ConstKeyIterator;
	typedef typename SuperType::UnselectedIterator UnselectedIterator;
	typedef typename SuperType::ConstUnselectedIterator ConstUnselectedIterator;



	BufferPool() : MultiPool<cl::Buffer, BufferKey<T> >() {}


	BufferPool(const BufferPool<T>& rhs) :
		MultiPool<cl::Buffer, BufferKey<T> >(rhs),
		_context(rhs._context) {}


	BufferPool(cl::Context& context) :
		MultiPool<cl::Buffer, BufferKey<T> >(),
		_context(context) {}


	UnselectedIterator find(size_t nelems, cl_mem_flags flags = CL_MEM_READ_WRITE) {
		std::vector<UnselectedIterator> ret;
		find(ret, 1, nelems, flags);
		return ret.empty() ? SuperType::invalid() : ret[0];
	}


	UnselectedIterator create(size_t nelems, cl_mem_flags flags = CL_MEM_READ_WRITE, void* ptr = NULL) {
		std::vector<UnselectedIterator> ret;
		create(ret, 1, nelems, flags, ptr);
		return ret.empty() ? SuperType::invalid() : ret[0];
	}


	UnselectedIterator findOrCreate(size_t nelems, cl_mem_flags flags = CL_MEM_READ_WRITE, void* ptr = NULL) {
		std::vector<UnselectedIterator> ret;
		findOrCreate(ret, 1, nelems, flags, ptr);
		return ret.empty() ? SuperType::invalid() : ret[0];
	}


	inline UnselectedIterator find(BufferKey<T> key) {
		return find(key.nelems(), key.flags());
	}


	inline UnselectedIterator create(BufferKey<T> key, void* ptr = NULL) {
		return create(key.nelems(), key.flags(), ptr);
	}


	inline UnselectedIterator findOrCreate(BufferKey<T> key, void* ptr = NULL) {
		return findOrCreate(key.nelems(), key.flags(), ptr);
	}


	template <class C> C& find(C& ret, size_t nbufs, size_t nelems, cl_mem_flags flags = CL_MEM_READ_WRITE) {
		size_t count = 0;
		typename SuperType::SelectionList unselected = SuperType::unselected();

		for (UnselectedIterator it = unselected.begin(); it != unselected.end() && count < nbufs; ++it) {
			BufferKey<T> it_key = *it;
			BufferKey<T> pm_key(nelems, flags);

			if (it_key == pm_key) {
				ret.insert(ret.end(), it);
				++count;
			}
		}

		return ret;
	}


	template <class C> C& create(C& ret, size_t nbufs, size_t nelems, cl_mem_flags flags = CL_MEM_READ_WRITE, void* ptr = NULL) {
		for (size_t i = 0; i < nbufs; ++i) {
			cl::Buffer buf(_context, flags, nelems * sizeof(ElemType), ptr);
			ret.insert(ret.end(), SuperType::addNew(buf));
		}
		return ret;
	}


	template <class C> C& findOrCreate(C& ret, size_t nbufs, size_t nelems, cl_mem_flags flags = CL_MEM_READ_WRITE, void* ptr = NULL) {
		find(ret, nbufs, nelems, flags);
		if (nbufs > ret.size())
			create(ret, nbufs - ret.size(), nelems, flags, ptr);
		return ret;
	}


	template <class C> inline C& find(C& ret, size_t nbufs, BufferKey<T> key) {
		return find(ret, nbufs, key.nelems(), key.flags());
	}


	template <class C> inline C& create(C& ret, size_t nbufs, BufferKey<T> key, void* ptr = NULL) {
		return create(ret, nbufs, key.nelems(), key.flags(), ptr);
	}


	template <class C> inline C& findOrCreate(C& ret, size_t nbufs, BufferKey<T> key, void* ptr = NULL) {
		return findOrCreate(ret, nbufs, key.nelems(), key.flags(), ptr);
	}


	cl::Context& context() {
		return _context;
	}


	BufferPool<T>* clone() {
		throw Error("Not implemented.");
	}


private:
	cl::Context _context;
};


class DevicePool;


class QueuePool : public ListPool<cl::CommandQueue> {
public:
	QueuePool() : ListPool<cl::CommandQueue>() {}


	QueuePool(const QueuePool& rhs) :
		ListPool<cl::CommandQueue>(rhs),
		_context(rhs._context),
		_device(rhs._device) {}


	QueuePool(cl::Context& context, cl::Device& device) :
		ListPool<cl::CommandQueue>(),
		_context(context),
		_device(device) {}


	Iterator find(cl_command_queue_properties properties = 0) {
		std::vector<Iterator> ret;
		find(ret, 1, properties);
		return ret.empty() ? invalid() : ret[0];
	}


	Iterator create(cl_command_queue_properties properties = 0) {
		std::vector<Iterator> ret;
		create(ret, 1, properties);
		return ret.empty() ? invalid() : ret[0];
	}


	Iterator findOrCreate(cl_command_queue_properties properties = 0) {
		std::vector<Iterator> ret;
		findOrCreate(ret, 1, properties);
		return ret.empty() ? invalid() : ret[0];
	}


	template <class C> C& find(C& ret, size_t nqueues, cl_command_queue_properties properties = 0) {
		Container& ctnr = unselected();
		size_t count = 0;

		for (Iterator it = ctnr.begin(); it != ctnr.end() && count < nqueues; ++it) {
			if (it->getInfo<CL_QUEUE_PROPERTIES>() == properties) {
				ret.insert(ret.end(), it);
				++count;
			}
		}

		return ret;
	}


	template <class C> C& create(C& ret, size_t nqueues, cl_command_queue_properties properties = 0) {
		for (size_t i = 0; i < nqueues; ++i) {
			Type queue(_context, _device, properties);
			Iterator it = addNew(queue);
			ret.insert(ret.end(), it);
		}
		return ret;
	}


	template <class C> C& findOrCreate(C& ret, size_t nqueues, cl_command_queue_properties properties = 0) {
		find(ret, nqueues, properties);
		if (nqueues > ret.size())
			create(ret, nqueues - ret.size(), properties);
		return ret;
	}


	cl::Context& context() {
		return _context;
	}


	cl::Device& device() {
		return _device;
	}


	void wait() {
		Container ctnr = all();
		for (Iterator it = ctnr.begin(); it != ctnr.end(); ++it)
			it->finish();
	}


	void flush() {
		Container ctnr = all();
		for (Iterator it = ctnr.begin(); it != ctnr.end(); ++it)
			it->flush();
	}


	QueuePool* clone() {
		throw Error("Not implemented.");
	}

private:
	cl::Context _context;
	cl::Device _device;
};


class DevicePool : public MapPool<cl::Device, QueuePool, Device_less> {
public:

	DevicePool() : MapPool<cl::Device, QueuePool, Device_less>() {}


	DevicePool(const DevicePool& rhs) : MapPool<cl::Device, QueuePool, Device_less>(rhs) {}


	DevicePool(cl::Context& context, std::vector<cl::Device>& devices) :
		MapPool<cl::Device, QueuePool, Device_less>(),
		_context(context)
	{
		for (std::vector<cl::Device>::iterator it = devices.begin(); it != devices.end(); ++it) {
			std::map<cl::Device, QueuePool, Device_less>::value_type entry(*it, QueuePool(_context, *it));
			addNew(entry);
		}
	}

	QueuePool& queues() {
		if (!hasSelected()) {
			throw Error("No device selected.");
		}
		return selected().second;
	}


	QueuePool& queues(Iterator& it) {
		if (it == invalid()) {
			throw Error("Invalid iterator.");
		}
		return it->second;
	}


	QueuePool& queues(cl::Device& device) {
		Iterator it = find(device);
		if (it == invalid()) {
			throw Error("Device not in the pool.");
		}
		return it->second;
	}


	cl::Context& context() {
		return _context;
	}


	template <class C> C& devices(C& ret) {
		Container ctnr = all();
		for (Iterator it = ctnr.begin(); it != ctnr.end(); ++it)
			ret.insert(ret.end(), it->first);
		return ret;
	}


	template <class C> C& queuePools(C& ret) {
		Container ctnr = all();
		for (Iterator it = ctnr.begin(); it != ctnr.end(); ++it)
			ret.insert(ret.end(), it->second);
		return ret;
	}


	void wait() {
		Container ctnr = all();
		for (Iterator it = ctnr.begin(); it != ctnr.end(); ++it)
			it->second.wait();
	}


	void flush() {
		Container ctnr = all();
		for (Iterator it = ctnr.begin(); it != ctnr.end(); ++it)
			it->second.flush();
	}


	DevicePool* clone() {
		throw Error("Not implemented.");
	}

private:
	cl::Context _context;
};


template <typename T> class State {
public:
	typedef ukoct::impl::opencl::ImplData<T> ImplData;
	typedef ukoct::impl::opencl::Operator<T> OperatorType;
	typedef std::map<ukoct::EOperation, OperatorType> OperatorMap;
	typedef typename OperatorMap::iterator OperatorIterator;


	State() :
		_valid(false),
		_size(0),
		_rowMajor(true) {}


	void setup(cl::Program& program, size_t diffSize, const std::string sourceFilename = "") {
		_program = program;
		_context = _program.getInfo<CL_PROGRAM_CONTEXT>();
		_devices = _program.getInfo<CL_PROGRAM_DEVICES>();
		_size = diffSize;
		_sourceFilename = sourceFilename;

		if (_size % 2 == 0 && _size > 1 && !_devices.empty()) {
			_valid = true;
			_devicePool = DevicePool(_context, _devices);
		}
	}


	cl::NDRange globalRange() {
		return cl::NDRange(_size, _size);
	}


	cl::NDRange globalKRange() {
		return cl::NDRange(_size, _size, _size);
	}


	cl::NDRange globalK2Range() {
		return cl::NDRange(_size, _size, _size - 2);
	}


	BufferKey<T> inputBufferKey() {
		return BufferKey<T>(_size * _size, CL_MEM_READ_WRITE);
	}


	cl::Buffer& inputBuffer() {
		return _bufferPool.selected(inputBufferKey());
	}


	const std::string& sourceFilename() const {
		return _sourceFilename;
	}


	const cl::Program& program() {
		return _program;
	}


	const cl::Context& context() {
		return _context;
	}


	const std::vector<cl::Device>& devices() {
		return _devices;
	}


	DevicePool& devicePool() {
		return _devicePool;
	}


	BufferPool<T>& bufferPool() {
		return _bufferPool;
	}


	std::map<EOperation, OperatorType>& operators() {
		return _operators;
	}


	bool valid() {
		return _valid;
	}


	size_t size() {
		return _size;
	}


	size_t octSize() {
		return _size / 2;
	}


	T hugeval() {
		return ImplData::hugeval();
	}


	bool rowMajor() {
		return _rowMajor;
	}


	State<T>* clone() {
#if 0
		State<T>* ret = new State<T>();
		ret->_valid = _valid;
		ret->_size = _size;
		ret->_program = _program;
		ret->_context = _context;
		ret->_devicePool = _devicePool.clone();
		ret->_bufferPool = _bufferPool.clone();
		for (std::map<EOperation, Operator<T> >::iterator it = _operators.begin(); it != _operators.end(); ++it) {
			ret->_operators[it->first] = it->second.copy();
		}
		return ret;
#endif
		throw Error("Not implemented.");
	}

private:
	cl::Context _context;
	cl::Program _program;
	DevicePool _devicePool;
	BufferPool<T> _bufferPool;
	std::vector<cl::Device> _devices;
	std::map<EOperation, OperatorType> _operators;
	bool _valid;
	bool _rowMajor;
	size_t _size;
	std::string _sourceFilename;
};


}
}

struct opencl {};

}



#endif /* UKOCT_OPENCL_BASE_HPP_ */
