#ifndef UKOCT_OPENCL_BASE_HPP_
#define UKOCT_OPENCL_BASE_HPP_


#include <cmath>
#include <limits>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <CL/cl.hpp>
#include "ukoct/core.hpp"
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
	static constexpr const char* source() { return ""; }
};


class OptimizedRangeCalculator {
public:
	OptimizedRangeCalculator() :
		_elemSize(0) {}
	OptimizedRangeCalculator(const OptimizedRangeCalculator& rhs) :
		_elemSize(rhs._elemSize),
		_local1(rhs._local1),
		_local2(rhs._local2),
		_local3(rhs._local3),
		_global1(rhs._global1),
		_global2(rhs._global2),
		_global3(rhs._global3) {}
	OptimizedRangeCalculator(cl::Device& device, size_t size, size_t elemSize) { calculate(device, size, elemSize); }

	inline const size_t elemSize() const { return _elemSize; }
	inline const cl::NDRange& local1() const { return _local1; }
	inline const cl::NDRange& local2() const { return _local2; }
	inline const cl::NDRange& local3() const { return _local3; }
	inline const cl::NDRange& global1() const { return _global1; }
	inline const cl::NDRange& global2() const { return _global2; }
	inline const cl::NDRange& global3() const { return _global3; }
	inline size_t localSize1() const { return ndrangeSize(_local1); }
	inline size_t localSize2() const { return ndrangeSize(_local2); }
	inline size_t localSize3() const { return ndrangeSize(_local3); }
	inline size_t globalSize1() const { return ndrangeSize(_global1); }
	inline size_t globalSize2() const { return ndrangeSize(_global2); }
	inline size_t globalSize3() const { return ndrangeSize(_global3); }
	inline size_t localElemSize1() const { return localSize1() * _elemSize; }
	inline size_t localElemSize2() const { return localSize2() * _elemSize; }
	inline size_t localElemSize3() const { return localSize3() * _elemSize; }
	inline size_t globalElemSize1() const { return globalSize1() * _elemSize; }
	inline size_t globalElemSize2() const { return globalSize2() * _elemSize; }
	inline size_t globalElemSize3() const { return globalSize3() * _elemSize; }

	void calculate(cl::Device& device, size_t size, size_t elemSize) {
		_elemSize = elemSize;
		size_t deviceMaxGroupSize = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
		std::vector<size_t> maxWorkItemSizes = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
		std::vector<size_t> localDims1(1), localDims2(2), localDims3(3);
		std::vector<size_t> globalDims1(1), globalDims2(2), globalDims3(3);
		size_t size_pow = highestPow2(size);

		//max group size should be powers of 2: deviceMaxGroupSize = highestPow2(deviceMaxGroupSize)
		//dimensions should be powers of 2: for (size_t i = 0; i < 3; ++i) maxWorkItemSizes[i] = highestPow2(maxWorkItemSizes[i]);
		//size should be even: size = size + (size & 1)

		localDims1[0] = std::min(size_pow, maxWorkItemSizes[0]);
		localDims2[0] = std::min(size_pow, maxWorkItemSizes[0]);
		localDims2[1] = std::min(size_pow, maxWorkItemSizes[1]);
		localDims3[0] = std::min(size_pow, maxWorkItemSizes[0]);
		localDims3[1] = std::min(size_pow, maxWorkItemSizes[1]);
		localDims3[2] = std::min(size_pow, maxWorkItemSizes[2]);

		while (totalRangeSize(localDims2) > deviceMaxGroupSize) {
			std::vector<size_t>::iterator max_elem = std::max_element(localDims2.begin(), localDims2.end());
			*max_elem >>= 1;
		}

		while (totalRangeSize(localDims3) > deviceMaxGroupSize) {
			std::vector<size_t>::iterator max_elem = std::max_element(localDims3.begin(), localDims3.end());
			*max_elem >>= 1;
		}

		_local1 = cl::NDRange(localDims1[0]);
		_local2 = cl::NDRange(localDims2[0], localDims2[1]);
		_local3 = cl::NDRange(localDims3[0], localDims3[1], localDims3[2]);
		_global1 = cl::NDRange(geMultiple(size, localDims1[0]));
		_global2 = cl::NDRange(
			geMultiple(size, localDims2[0]),
			geMultiple(size, localDims2[1])
		);
		_global3 = cl::NDRange(
			geMultiple(size, localDims3[0]),
			geMultiple(size, localDims3[1]),
			geMultiple(size, localDims3[2])
		);
	}

private:
	size_t highestPow2(size_t num) {
		size_t ret = 1;
		while (ret < num) ret <<= 1;
		return ret;
	}
	size_t geMultiple(size_t size, size_t dimSize) {
		size_t v = 1;
		while (v * dimSize < size) ++v;
		return size * v;
	}
	size_t totalRangeSize(const std::vector<size_t>& rng) {
		size_t ret = rng[0];
		for (std::vector<size_t>::const_iterator it = rng.begin() + 1; it != rng.end(); ++it)
			ret *= *it;
		return ret;
	}
	size_t ndrangeSize(const cl::NDRange& rng) const {
		const size_t* data = rng;
		size_t sz = data[0];
		for (size_t i = 1; i < rng.dimensions(); ++i)
			sz *= data[i];
		return sz;
	}
private:
	size_t _elemSize;
	cl::NDRange _local1;
	cl::NDRange _local2;
	cl::NDRange _local3;
	cl::NDRange _global1;
	cl::NDRange _global2;
	cl::NDRange _global3;
};


template <typename T> class State {
public:

	State() :
		_valid(false),
		_size(0),
		_ranges(),
		_rowMajor(true),
		_context(),
		_program(),
		_device(),
		_queues(),
		_mat(),
		_aux(),
		_val() {}


	State(const State<T>& rhs) :
		_valid(rhs._valid),
		_size(rhs._size),
		_ranges(rhs._ranges),
		_rowMajor(rhs._rowMajor),
		_context(rhs._context),
		_program(rhs._program),
		_device(rhs._device),
		_queues(rhs._queues),
		_mat(rhs._mat),
		_aux(rhs._aux),
		_val(rhs._val) {}


	void setup(cl::Program& program, size_t size, bool rowMajor = true, T* data = NULL, cl_mem_flags flags = 0) {
		std::vector<cl::Device> devices = _program.getInfo<CL_PROGRAM_DEVICES>();

		if (devices.empty())
			throw Error("No available device in the program info.");

		if (_size % 2 == 0 && _size > 1)
			throw Error("Incorrect size for DBM. Size must be greater than 1 and a multiple of 2.");

		if (_program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) != CL_BUILD_SUCCESS)
			throw Error("Program not built.");

		_program = program;
		_size = size;
		_context = _program.getInfo<CL_PROGRAM_CONTEXT>();
		_device = devices[0];
		_val = T();
		_ranges.calculate(_device, _size, sizeof(T));
		queues(1);
		if (data != NULL && flags == 0)
			flags = CL_MEM_COPY_HOST_PTR;
		_mat = cl::Buffer(_context, CL_MEM_READ_WRITE | flags, bytesize(), data);
		_aux = cl::Buffer(_context, CL_MEM_READ_WRITE, auxBytesize());

		_valid = true;
	}

	bool valid() const { return _valid; }

	size_t size() const { return _size; }

	const OptimizedRangeCalculator& ranges() const { return _ranges; }

	size_t elemsize() const { return _size * _size; }

	size_t auxElemsize() const { return _size * _size * _size; }

	size_t bytesize() const { return elemsize() * sizeof(T); }

	size_t auxBytesize() const { return _ranges().globalElemSize3(); }

	size_t octsize() const { return _size / 2; }

	bool rowMajor() const { return _rowMajor; }

	cl::NDRange globalRange1() const { return cl::NDRange(_size); }

	cl::NDRange globalRange() const { return cl::NDRange(_size, _size); }

	cl::NDRange globalRange3() const { return cl::NDRange(_size, _size, _size); }

	const cl::Context& context() const { return _context; }

	const cl::Program& program() const { return _program; }

	const cl::Device& device() const { return _device; }

	std::vector<cl::CommandQueue>& queues() { return _queues; }
	const std::vector<cl::CommandQueue>& queues() const { return _queues; }
	State<T>& queues(const std::vector<cl::CommandQueue>& q) { _queues = q; return *this; }
	std::vector<cl::CommandQueue>& queues(size_t size) {
		if (size > _queues.size())
			for (size_t i = 0; i < size - _queues.size(); ++i) {
				cl::CommandQueue queue(_context, _device, 0, bytesize());
				_queues.push_back(queue);
			}
		return _queues;
	}

	//cl::Buffer& mat() { return _mat; }
	const cl::Buffer& mat() const { return _mat; }

	//cl::Buffer& aux() { return _aux; }
	const cl::Buffer& aux() const { return _aux; }

	T& val() { return _val; }
	const T& val() const { return _val; }
	State<T>& val(T val) { _val = val; return *this; }
	State<T>& val(const T& val) { _val = val; return *this; }

private:
	bool _valid;
	OptimizedRangeCalculator _ranges;
	size_t _size;
	bool _rowMajor;
	cl::Context _context;
	cl::Program _program;
	cl::Device _device;
	std::vector<cl::CommandQueue> _queues;
	cl::Buffer _mat;
	cl::Buffer _aux;
	T _val;
};


typedef void (*FContextCallback)(const char* message, const void* data, size_t dataSize, void* userData);


static cl::Context createContext(size_t numDevices = 0, cl_device_type deviceType = CL_DEVICE_TYPE_ALL, FContextCallback callback = NULL, void* callbackData = NULL) {
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.empty())
		throw Error("No OpenCL Platform found, please check your OpenCL installation.");

	bool foundDevice = false;
	std::vector<cl::Device> devices;
	for (std::vector<cl::Platform>::iterator it = platforms.begin(); !foundDevice && it != platforms.end(); ++it) {
		cl::Platform platform = *it;
		std::vector<cl::Device> platformDevices;
		platform.getDevices(deviceType, &platformDevices);

		if (!platformDevices.empty() && platformDevices.size() >= numDevices) {
			foundDevice = true;
			if (numDevices == 0)
				numDevices = platformDevices.size();
			devices.insert(devices.end(), platformDevices.begin(), platformDevices.begin() + numDevices);
		}
	}
	if (!foundDevice)
		throw Error("Could not find available/enough devices.");

	cl::Context context(devices, NULL, callback, callbackData);
	return context;
}


static cl::Program createProgramFromSource(cl::Context context, const char* source, size_t sourceSize, const char* options) {
	std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

	if (devices.empty())
		throw Error("No device available in context.");

	cl::Program::Sources sources;
	sources.push_back(std::make_pair(source, sourceSize));
	cl::Program program(context, sources);
	program.build(devices, options);
	for (auto& device : devices) {
		if (program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device) != CL_BUILD_SUCCESS) {
			std::stringstream ss;
			ss
				<< "Program could not be built for device \"" << device.getInfo<CL_DEVICE_NAME>() << "\"."
				<< " Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device)
				<< ", Options: " << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(device)
				<< ", Log: \"" << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\".";
			throw Error(ss.str());
		}
	}

	return program;
}


static cl::Program createProgramFromSource(cl::Context context, std::string source, const char* options = NULL) {
	return createProgramFromSource(context, source.c_str(), source.length(), options);
}


static cl::Program createProgramFromSource(cl::Context context, const std::string& source, const char* options = NULL) {
	return createProgramFromSource(context, source.c_str(), source.length(), options);
}


}
}

struct opencl {};

}



#endif /* UKOCT_OPENCL_BASE_HPP_ */
