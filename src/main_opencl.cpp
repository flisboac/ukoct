#include "main.hpp"


//
// FLOAT
//


class OpenclFloatCallback : public IOperationCallback {
	bool operator()(ArgState& A, Operand& operand) const {
		A.warn() << "The backend for '" << elemNames[A.elemType].name << "' on '" << implNames[A.implType].name << "' is not implemented yet, sorry!" << std::endl;
		return true;
	}
};

const OpenclFloatCallback openclFloatCallback;
const IOperationCallback* const ocb_opencl_flt = &openclFloatCallback;
