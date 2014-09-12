#include "main.hpp"

//
// FLOAT
//


class CpuFloatCallback : public IOperationCallback {
	bool operator()(ArgState& A, Operand& operand) const {
		A.warn() << "The backend for '" << elemNames[A.elemType].name << "' on '" << implNames[A.implType].name << "' is not implemented yet, sorry!" << std::endl;
		return true;
	}
};

const CpuFloatCallback cpuFloatCallback;
const IOperationCallback* const ocb_cpu_flt = &cpuFloatCallback;

//
// DOUBLE
//


class CpuDoubleCallback : public IOperationCallback {
	bool operator()(ArgState& A, Operand& operand) const {
		A.warn() << "The backend for '" << elemNames[A.elemType].name << "' on '" << implNames[A.implType].name << "' is not implemented yet, sorry!" << std::endl;
		return true;
	}
};

const CpuDoubleCallback cpuDoubleCallback;
const IOperationCallback* const ocb_cpu_dbl = &cpuDoubleCallback;



//
// LONG DOUBLE
//


class CpuLDoubleCallback : public IOperationCallback {
	bool operator()(ArgState& A, Operand& operand) const {
		A.warn() << "The backend for '" << elemNames[A.elemType].name << "' on '" << implNames[A.implType].name << "' is not implemented yet, sorry!" << std::endl;
		return true;
	}
};

const CpuLDoubleCallback cpuLDoubleCallback;
const IOperationCallback* const ocb_cpu_ldbl = &cpuLDoubleCallback;
