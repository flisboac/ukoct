#ifndef UKOCT_OPENCL_COMMON_OPERATORS_ABSTRACT_HPP_
#define UKOCT_OPENCL_COMMON_OPERATORS_ABSTRACT_HPP_

#include "ukoct/opencl/common/operator.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {


template <typename T, class C> class AbstractOperator : public IOperator<T>, public IEventCallback {
public:
	virtual ~AbstractOperator() {}

	virtual C* instantiate() {
		return new C;
	}

	//virtual ukoct::EOperation operation() = 0;
	virtual const char* kernelFunctionName() = 0;
	virtual void enqueueEvents(typename IOperator<T>::Args& args) = 0;

	virtual cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel(args.state().program(), kernelFunctionName());
		kernel.setArg(nextArgIdx++, args.state().mat());
		kernel.setArg(nextArgIdx++, args.state().rowMajor());
		return kernel;
	}

	virtual void prepareResult(typename IOperator<T>::Args& args) {

	}

	virtual void setCallbacks(typename IOperator<T>::Args& args) {
		if (args.waiting()) {
			cl::WaitForEvents(_result.events());
			eventCallback(0, 0);
		}
	}

	virtual void run(typename IOperator<T>::Args& args) {
		prepareResult(args);
		enqueueEvents(args);
		setCallbacks(args);
	}


	virtual const typename IOperator<T>::Result& result() {
		return _result;
	}


	virtual void eventCallback(cl_event event, cl_int status) {
		_result.boolean(_result.result() != 0);
	}


protected:
	typename IOperator<T>::Result _result;
};


template <typename T, class C> class AbstractGlobalOperator : public AbstractOperator<T, C> {
public:
	virtual ~AbstractGlobalOperator() {}

	virtual cl::NDRange globalNDRange(typename IOperator<T>::Args& args) {
		return args.state().globalRange();
	}

	virtual void enqueueEvents(typename IOperator<T>::Args& args) {
		State<T>& state = args.state();
		typename IOperator<T>::Result& result = AbstractOperator<T, C>::_result;
		std::vector<cl::Event> waitEvents = args.waitEvents();

		result.events().resize(1);
		result.queues().push_back(state.queues(1)[0]);

		size_t kernelArgIdx = 0;
		cl::Kernel kernel = createKernel(args, kernelArgIdx);
		result.queues()[0].enqueueNDRangeKernel(kernel, cl::NullRange, globalNDRange(args), cl::NullRange, (waitEvents.empty() ? NULL : &waitEvents), &result.events()[0]);
	}
};


template <typename T, class C> class AbstractGlobalAuxOperator : public AbstractGlobalOperator<T, C> {
public:
	virtual ~AbstractGlobalAuxOperator() {}

	virtual cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel = AbstractOperator<T, C>::createKernel(args, &nextArgIdx);
		kernel.setArg(nextArgIdx++, args.size());
		kernel.setArg(nextArgIdx++, args.state().aux());
		return kernel;
	}

	virtual void enqueueEvents(typename IOperator<T>::Args& args) {
		AbstractGlobalOperator<T, C>::enqueueEvents(args);
		typename IOperator<T>::Result& result = AbstractGlobalOperator<T, C>::_result;
		std::vector<cl::Event> events = result.events();
		result.events().resize(result.events().size() + 1);
		result.queues()[0].enqueueReadBuffer(args.aux(), false, 0, 1, &result.result(), &events, result.events().back());
	}
};


template <typename T, class C> class AbstractGlobalBinaryOperator : public AbstractGlobalOperator<T, C> {
public:
	virtual ~AbstractGlobalBinaryOperator() {}

	virtual cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel = AbstractOperator<T, C>::createKernel(args, &nextArgIdx);
		State<T>& otherState = *args.otherState();
		kernel.setArg(nextArgIdx++, otherState.mat());
		kernel.setArg(nextArgIdx++, otherState.rowMajor());
		return kernel;
	}
};


template <typename T, class C> class AbstractGlobalAuxBinaryOperator : public AbstractGlobalBinaryOperator<T, C> {
public:
	virtual ~AbstractGlobalAuxBinaryOperator() {}

	virtual cl::Kernel createKernel(typename IOperator<T>::Args& args, size_t& nextArgIdx) {
		cl::Kernel kernel = AbstractOperator<T, C>::createKernel(args, &nextArgIdx);
		kernel.setArg(nextArgIdx++, args.state().aux());
		return kernel;
	}

	virtual void enqueueEvents(typename IOperator<T>::Args& args) {
		AbstractGlobalBinaryOperator<T, C>::enqueueEvents(args);
		typename IOperator<T>::Result& result = AbstractGlobalOperator<T, C>::_result;
		std::vector<cl::Event> events = result.events();
		result.events().resize(result.events().size() + 1);
		result.queues()[0].enqueueReadBuffer(args.aux(), false, 0, 1, &result.result(), &events, result.events().back());
	}
};

}
}
}
}


#endif /* UKOCT_OPENCL_COMMON_OPERATORS_ABSTRACT_HPP_ */
