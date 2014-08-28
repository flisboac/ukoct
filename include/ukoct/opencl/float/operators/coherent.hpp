#ifndef UKOCT_OPENCL_FLOAT_OPERATORS_COHERENT_HPP_
#define UKOCT_OPENCL_FLOAT_OPERATORS_COHERENT_HPP_

#include "ukoct/opencl/common/operator.hpp"

namespace ukoct {
namespace impl {
namespace opencl {
namespace octdiff {

template <typename T> class CoherentOperator_Global_Reduce : public IOperator<T>, public IEventCallback {
public:
	static constexpr char* FunctionName = "octdiff_coherent__global_reduce";

	CoherentOperator_Global_Reduce* instantiate() {
		return new CoherentOperator_Global_Reduce;
	}

	ukoct::EOperation operation() {
		return ukoct::OPER_COHERENT;
	}

	void run(typename IOperator<T>::Args& args) {
		// Get kernel arguments
		State<T>& state = args.state();
		cl::Program& program = state.program();
		cl::Buffer input = state.inputBuffer();
		cl::Buffer output = *state.bufferPool().findOrCreate(state.inputBufferKey());
		std::vector<cl::CommandQueue>& allQueues = _result.queues();
		std::vector<cl::Event>& allEvents = _result.events();
		std::vector<cl::Event> events(1);

		allEvents.resize(2);
		allQueues.push_back(state.devicePool().queues().selected());

		// Create kernel and enqueue it
		cl::Kernel kernel(program, FunctionName);
		kernel.setArg(0, input);
		kernel.setArg(1, result);
		kernel.setArg(2, state.rowMajor());
		allQueues[0].enqueueNDRangeKernel(kernel, cl::NullRange, state.globalRange(), cl::NullRange, NULL, &events[0]);
		allEvents[0] = events[0];

		// Wait or set callback depending on the waiting flag
		if (args.waiting()) {
			T flag;
			allQueues[0].enqueueReadBuffer(data->buffer, true, 0, 1, &flag, &events, &allEvents[1]);
			cl::WaitForEvents(allEvents);
			_result.boolean() = (flag != 0);

		} else {
			allQueues[0].enqueueReadBuffer(data->buffer, false, 0, 1, &_result.result(), &events, &allEvents[1]);
		}

		allQueues[0].flush();
	}


	const Result& result() {
		return _result;
	}


	void eventCallback(cl_event event, cl_int status) {
		_result.boolean() = (_result.result() != 0);
	}


private:
	Result _result;
};

}
}
}
}


#endif /* UKOCT_OPENCL_FLOAT_OPERATORS_COHERENT_HPP_ */
