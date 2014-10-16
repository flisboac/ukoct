#include "plas.hpp"
#include "ukoct/cpu.hpp"
#include "main.hpp"


template <typename T> class CpuCallback : public IOperationCallback {
public:
	bool operator()(ArgState& A, Operand& O) const {
		bool result = true;

		switch (O.stage) {
		case STAGE_INIT:
			result = init(A, O);
			break;

		case STAGE_RUN:
			result = execute(A, O);
			break;

		case STAGE_FINISH:
			finish(A, O);
			break;

		default:
			ukoct_DBG(throw ukoct::Error("Invalid operand stage for operand callback."));
			break;
		}

		return true;
	}

private:
	bool init(ArgState& A, Operand& O) const {
		plas::Problem<T, plas::DenseMatrix>* problem = loadProblem<T>(A, O);
		bool ret = problem != NULL;

		if (ret) {
			ukoct::CpuState<T>* state = new ukoct::CpuState<T>();
			plas::IMatrix<T>& mat = *problem->matrix();
			bool rowMajor = mat.ordering() == plas::MATRIX_ROWMAJOR ? true : false;
			state->setup(mat.colsize(), mat.raw(), rowMajor);
			O.state = state;
			delete problem;
		}

		return ret;
	}

	void finish(ArgState& A, Operand& O) const {
		finalizeProblem<T>(A, O);
		ukoct::CpuState<T>* state = reinterpret_cast<ukoct::CpuState<T>*>(O.state);
		delete state;
		O.state = NULL;
	}

	bool execute(ArgState& A, Operand& O) const {
		bool ret = true;
		bool keepgoing = true;
		const Option& option = options[O.optionId];
		ukoct::EOperation operation = option.operation;
		const ukoct::IOperator<T>* oper = NULL;
		ukoct::CpuState<T>& state = *reinterpret_cast<ukoct::CpuState<T>*>(A.inputOperand.state);
		ukoct::CpuState<T>* other = reinterpret_cast<ukoct::CpuState<T>*>(O.state);
		plas::OctDiffConstraint<T>* diffCons = reinterpret_cast<plas::OctDiffConstraint<T>*>(O.diffCons);
		plas::OctConstraint<T>* octCons = reinterpret_cast<plas::OctConstraint<T>*>(O.octCons);

		switch(O.optionId) {
		case OPT_OP_ECHO:
			A.err() << "echo not implemented for this implementation and element type yet, sorry!" << std::endl;
			A.exitCode = RETNOTIMPL;
			keepgoing = false;
			break;

		case OPT_OP_NEQ:
			operation = ukoct::OPER_EQUALS;
			break;
		}

		if (keepgoing) {
			std::vector<const ukoct::IOperator<T>*> operators;
			state.implementation().filterOperators(operators, 1, operation, A.variants[operation].details);
			if (operators.empty()) {
				oper = state.implementation().getOptimalOperator(operation);
			} else {
				oper = operators.back();
			}

			if (oper == NULL) {
				keepgoing = false;
			}
		}

		if (keepgoing) {
			ukoct::OperatorArgs<T> args(state);
			args.other(other);
			args.var(O.var);
			if (diffCons != NULL) args.diffCons(*diffCons);
			if (octCons != NULL)  args.octCons(*octCons);
			ukoct::IOperatorResult<T>* result = oper->run(args);
			result->wait();
			ukoct_ASSERT(result->isFinished(), "Result should be finished after call to wait().");
			switch (operation) {
			//case ukoct::OPER_NONE:
			//case ukoct::OPER_COPY:
			case ukoct::OPER_ISCONSISTENT:
			case ukoct::OPER_ISINTCONSISTENT:
			case ukoct::OPER_ISCOHERENT:
			case ukoct::OPER_ISCLOSED:
			case ukoct::OPER_ISSTRONGLYCLOSED:
			case ukoct::OPER_ISTIGHTLYCLOSED:
			case ukoct::OPER_ISWEAKLYCLOSED:
			case ukoct::OPER_ISTOP:
			case ukoct::OPER_CLOSURE:
			case ukoct::OPER_TIGHTCLOSURE:
			//case ukoct::OPER_SHORTESTPATH:
			//case ukoct::OPER_STRENGTHEN:
			//case ukoct::OPER_TIGHTEN:
			//case ukoct::OPER_TOP:
			//case ukoct::OPER_PUSHDIFFCONS:
			//case ukoct::OPER_PUSHOCTCONS:
			//case ukoct::OPER_FORGETOCTVAR:
			case ukoct::OPER_EQUALS:
			case ukoct::OPER_INCLUDES:
			//case ukoct::OPER_UNION:
			//case ukoct::OPER_INTERSECTION:
				ret = (O.optionId == OPT_OP_NEQ ? !result->boolResult() : result->boolResult());
				break;
			}
			delete result;
		}

		return ret;
	}
};

const CpuCallback<float> cpuFloatCallback;
const IOperationCallback* const ocb_cpu_flt = &cpuFloatCallback;

const CpuCallback<double> cpuDoubleCallback;
const IOperationCallback* const ocb_cpu_dbl = &cpuDoubleCallback;

const CpuCallback<long double> cpuLDoubleCallback;
const IOperationCallback* const ocb_cpu_ldbl = &cpuLDoubleCallback;
