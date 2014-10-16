#ifndef UKOCT_CPU_OPERATORS_ABSTRACT_HPP_
#define UKOCT_CPU_OPERATORS_ABSTRACT_HPP_

#include <vector>

#include "ukoct/core/defs.hpp"
#include "ukoct/cpu/base.hpp"

namespace ukoct {
namespace impl {
namespace cpu {
namespace octdiff {


template <typename T> struct CpuResult {
	bool boolResult;
	std::vector<Timing> timings;
};

template <typename T> class AbstractCpuOperator : public IOperator<T> {
public:
	virtual ~AbstractCpuOperator() {}


	bool isFinished() {
		return true;
	}


	void wait() {}


	bool boolResult() const {
		return _result.boolResult;
	}


	const std::vector<Timing>& timings() const {
		return _result.timings;
	}


	void run(OperatorArgs<T>& args) const {
		run(args, _result);
	}


	virtual void run(const OperatorArgs<T>& args, CpuResult<T>& ret) const = 0;


protected:
	void start(CpuTiming& timing) {
		_result.timing.start();
	}


	void end(CpuTiming& timing) {
		timing.end();
		_result.timings.push_back(timing);
	}


protected:
	CpuResult<T> _result;
};


}
}
}
}

#endif /* UKOCT_CPU_OPERATORS_ABSTRACT_HPP_ */
