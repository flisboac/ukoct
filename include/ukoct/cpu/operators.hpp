#ifndef UKOCT_CPU_OPERATORS_HPP_
#define UKOCT_CPU_OPERATORS_HPP_

#include <cmath>

#include "plas.hpp"
#include "ukoct/cpu/base.hpp"

namespace ukoct {
namespace impl {
namespace cpu {

template <typename T> class AbstractCpuOperator;

template <typename T> class CpuOperatorResult : public IOperatorResult<T> {
public:
	CpuOperatorResult(const AbstractCpuOperator<T>& oper) : _boolResult(false), _oper(oper) {}
	inline const CpuImplementation<T>& implementation() const { return ukoct::cpu<T>::impl; }
	inline const AbstractCpuOperator<T>& oper() const { return _oper; }
	inline bool isFinished() const { return true; }
	inline bool boolResult() const { return _boolResult; }
	inline void wait() {}

	inline CpuOperatorResult<T>& boolResult(bool v) { _boolResult = v; return *this; }

private:
	const IOperator<T>& _oper;
	bool _boolResult;
};


template <typename T> class AbstractCpuOperator : public IOperator<T> {
public:
	virtual ~AbstractCpuOperator() {}
	inline const CpuImplementation<T>& implementation() const { return ukoct::cpu<T>::impl; }
	ukoct::OperationDetails details() const { return O_DIMS_EXACT | O_EXEC_LOOP; }
	CpuOperatorResult<T>* run(const OperatorArgs<T>& args) const {
		CpuOperatorResult<T>* ret = new CpuOperatorResult<T>();
		ret->boolResult(true);
		return run(args, ret);
	}

	virtual CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const = 0;
};


template <typename T> class IsConsistentCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISCONSISTENT; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		ret->boolResult(true);

		for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k) {
			if (mat(k, k) < 0)
				ret->boolResult(false);
			else
				mat(k, k) = 0;
		}

		return ret;
	}
};


template <typename T> class IsIntConsistentCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISINTCONSISTENT; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		ret->boolResult(true);

		for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k) {
			plas::var_t K = plas::switchVar(k);
			if (mat(k, K) + mat(K, k) < 0)
				return false;
		}

		return ret;
	}
};


template <typename T> class IsCoherentCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISCOHERENT; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		ret->boolResult(true);

		for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i) {
			for (plas::var_t j = 1; i <= state.diffSize(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				if (mat(i, j) != mat(J, I))
					ret->boolResult(false);
			}
		}

		return ret;
	}
};


template <typename T> class IsClosedCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISCLOSED; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		IsConsistentCpuOperator<T> isConsistent;
		isConsistent.run(args, ret);

		if (ret->boolResult()) {
			ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
			plas::DenseMatrix<T>& mat = state.input();

			for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i) {
				for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j)
					for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k)
						if (mat(i, j) > mat(i, k) + mat(k, j))
							return false;
			}
		}

		return ret;
	}
};


template <typename T> class IsStronglyClosedCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISSTRONGLYCLOSED; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		IsConsistentCpuOperator<T> isConsistent;
		isConsistent.run(args, ret);

		if (ret->boolResult()) {
			ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
			plas::DenseMatrix<T>& mat = state.input();

			for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i) {
				for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j) {
					plas::var_t I = plas::switchVar(i);
					plas::var_t J = plas::switchVar(j);

					if (mat(i, j) > (mat(i, I) + mat(J, j)) / 2)
						ret->boolResult(false);

					for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k)
						if (mat(i, j) > mat(i, k) + mat(k, j))
							ret->boolResult(false);
				}
			}
		}

		return ret;
	}
};


template <typename T> class IsTightlyClosedCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISTIGHTLYCLOSED; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		ret->boolResult(true);

		for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k) {
			plas::var_t K = plas::switchVar(k);

			// for all i, m(i, i) = 0
			if (mat(k, k) < 0)
				ret->boolResult(false);
			else
				mat(k, k) = 0;

			// for all i, m(i, I) is even
			if (mat(k, K) & 1) // _mat(i, I) % 2 != 0
				ret->boolResult(false);
		}

		for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i) {
			for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				// for all i j, m[ij] <= (m[iI] + m[Jj]) / 2
				if (mat(i, j) > (mat(i, I) + mat(J, j)) / 2)
					ret->boolResult(false);

				for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k)
					if (mat(i, j) > mat(i, k) + mat(k, j))
						ret->boolResult(false);
			}
		}

		return ret;
	}
};


template <typename T> class IsWeaklyClosedCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISWEAKLYCLOSED; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		ret->boolResult(true);

		for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k) {
			if (mat(k, k) < 0)
				ret->boolResult(false);
			else
				mat(k, k) = 0;
		}

		for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i) {
			for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				for (plas::var_t k = 1; k <= state.diffSize() && ret->boolResult(); ++k) {
					if (mat(i, k) + mat(k, j) < std::min(mat(i, j), (mat(i, I) + mat(J, j)) / 2))
						ret->boolResult(false);
				}
			}
		}

		return ret;
	}
};


template <typename T> class IsTopCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_ISTOP; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		T infinity = AbstractCpuOperator<T>::implementation().infinity();
		ret->boolResult(true);

		for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i)
			for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j)
				if (i != j && mat(i, j) != infinity)
					ret->boolResult(false);

		return ret;
	}
};


template <typename T> class ShortestPathCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_SHORTESTPATH; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		size_t iters = args.iterations();

		if (iters == 0)
			iters = state.diffSize();

		if (iters > state.diffSize() / 2)
			iters = state.diffSize() / 2;

		for (size_t k = 1; k <= iters; ++k, ++k) {
			plas::var_t K = plas::switchVar(k);

			for (plas::var_t i = 1; i <= state.diffSize(); ++i)
				for (plas::var_t j = 1; j <= state.diffSize(); ++j) {
					plas::var_t I = plas::switchVar(i);
					plas::var_t J = plas::switchVar(j);
					mat(i, j) = std::min(
						mat(i, j), std::min(                              // i -> j
							mat(i, k) + mat(k, j), std::min(             // i -> k -> j
								mat(i, K) + mat(K, j), std::min(         // i -> K -> j
									mat(i, k) + mat(k, K) + mat(K, j),  // i -> k -> K -> j
									mat(i, K) + mat(K, k) + mat(k, j)   // i -> K -> k -> j
								)
							)
						)
					);
				}

			// QUESTION: Is it needed to strengthen right after shortest path, as in miné's paper,
			// or can it be left to closure?
			// TODO strengthen here.
		}

		return ret;
	}
};


template <typename T> class StrengthenCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_STRENGTHEN; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		bool intBased = args.intBased();

		for (plas::var_t i = 1; i <= state.diffSize(); ++i)
			for (plas::var_t j = 1; j <= state.diffSize(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);
				if (!intBased)
					mat(i, j) = std::min(mat(i, j), (mat(i, I) + mat(J, j)) / 2);

				else {
					if (i == j)
						mat(i, j) = std::min(mat(i, j), std::floor((mat(i, I) + mat(J, j)) / 2));
					else
						mat(i, j) = 2 * std::floor(mat(i, I) / 2);
				}
			}

		return ret;
	}
};


template <typename T> class TightenCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_TIGHTEN; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();

		for (plas::var_t i = 1; i <= state.diffSize(); ++i) {
			plas::var_t I = plas::switchVar(i);
			mat(i, I) = 2 * std::floor(mat(i, I) / 2);
		}

		return ret;
	}
};


template <typename T> class TopCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_TIGHTEN; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		T infinity = AbstractCpuOperator<T>::implementation().infinity();

		for (plas::var_t i = 1; i <= state.diffSize(); ++i)
			for (plas::var_t j = 1; j <= state.diffSize(); ++j)
				if (i == j)
					mat(i, j) = 0;
				else
					mat(i, j) = infinity;


		return ret;
	}
};

template <typename T> class ClosureCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_CLOSURE; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ShortestPathCpuOperator<T> shortestPath;
		IsConsistentCpuOperator<T> isConsistent;
		StrengthenCpuOperator<T> strengthen;

		shortestPath.run(args, ret);
		isConsistent.run(args, ret);

		if (ret->boolResult())
			strengthen.run(args, ret);

		return ret;
	}
};


template <typename T> class TightClosureCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_TIGHTCLOSURE; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ShortestPathCpuOperator<T> shortestPath;
		IsConsistentCpuOperator<T> isConsistent;
		IsIntConsistentCpuOperator<T> isIntConsistent;
		TightenCpuOperator<T> tighten;

		shortestPath.run(args, ret);
		isConsistent.run(args, ret);

		if (ret->boolResult()) {
			tighten.run(args, ret);
			isIntConsistent.run(args, ret);
		}

		return ret;
	}
};


template <typename T> class IncludesCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_INCLUDES; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct_ASSERT(args.other() != NULL, "Secondary matrix must be provided for this operator.");
		ukoct_ASSERT(AbstractCpuOperator<T>::implementation() != args.other()->implementation(), "Both matrices need to be from the same implementation.");
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		ukoct::CpuState<T>& other = *reinterpret_cast<CpuState<T>*>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		plas::DenseMatrix<T>& oth = other.input();
		ret->boolResult(true);

		if (&state != &other) {
			if (state.diffSize() != other.diffSize())
				ret->boolResult(false);
				//throw Error("Problem sizes cannot be different.");

			for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i)
				for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j)
					if (oth(i, j) > mat(i, j))
						ret->boolResult(false);
		}


		return ret;
	}
};


template <typename T> class EqualsCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_EQUALS; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct_ASSERT(args.other() != NULL, "Secondary matrix must be provided for this operator.");
		ukoct_ASSERT(AbstractCpuOperator<T>::implementation() != args.other()->implementation(), "Both matrices need to be from the same implementation.");
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		ukoct::CpuState<T>& other = *reinterpret_cast<CpuState<T>*>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		plas::DenseMatrix<T>& oth = other.input();
		ret->boolResult(true);

		if (&state != &other) {
			if (state.diffSize() != other.diffSize())
				ret->boolResult(false);
				//throw Error("Problem sizes cannot be different.");

			for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i)
				for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j)
					if (mat(i, j) != oth(i, j))
						ret->boolResult(false);
		}


		return ret;
	}
};


template <typename T> class UnionCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_UNION; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct_ASSERT(args.other() != NULL, "Secondary matrix must be provided for this operator.");
		ukoct_ASSERT(AbstractCpuOperator<T>::implementation() != args.other()->implementation(), "Both matrices need to be from the same implementation.");
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		ukoct::CpuState<T>& other = *reinterpret_cast<CpuState<T>*>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		plas::DenseMatrix<T>& oth = other.input();
		ret->boolResult(true);

		if (&state != &other) {
			if (state.diffSize() != other.diffSize())
				//ret->boolResult(false);
				throw Error("Problem sizes cannot be different.");

			for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i)
				for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j)
					mat(i, j) = std::max(mat(i, j), oth(i, j));
		}


		return ret;
	}
};


template <typename T> class IntersectionCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_INTERSECTION; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct_ASSERT(args.other() != NULL, "Secondary matrix must be provided for this operator.");
		ukoct_ASSERT(AbstractCpuOperator<T>::implementation() != args.other()->implementation(), "Both matrices need to be from the same implementation.");
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		ukoct::CpuState<T>& other = *reinterpret_cast<CpuState<T>*>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		plas::DenseMatrix<T>& oth = other.input();
		ret->boolResult(true);

		if (&state != &other) {
			if (state.diffSize() != other.diffSize())
				//ret->boolResult(false);
				throw Error("Problem sizes cannot be different.");

			for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i)
				for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j)
					mat(i, j) = std::min(mat(i, j), oth(i, j));
		}


		return ret;
	}
};


template <typename T> class PushDiffConsCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_PUSHDIFFCONS; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::OctDiffConstraint<T> f = args.diffCons();
		plas::DenseMatrix<T>& mat = state.input();

		for (plas::var_t i = 1; i <= state.diffSize() && ret->boolResult(); ++i)
			for (plas::var_t j = 1; j <= state.diffSize() && ret->boolResult(); ++j)
				mat(i, j) = std::min(mat(i, j), mat(i, f.a()) + f.d() + mat(f.b(), j));

		return ret;
	}
};


template <typename T> class PushOctConsCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_PUSHOCTCONS; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::OctDiffConstraint<T> ca, cb;
		plas::DenseMatrix<T>& mat = state.input();
		args.octcons().split(ca, cb);

		for (plas::var_t i = 1; i < state.diffSize(); ++i) {
			for (plas::var_t j = 1; j < state.diffSize(); ++j) {
				T da = mat(i, ca.a()) + ca.d() + mat(ca.b(), j);
				T db = da;
				if (cb.valid())
					db = mat(i, cb.a()) + cb.d() + mat(cb.b(), j);
				mat(i, j) = std::min(mat(i, j), std::min(da, db));
			}
		}

		return ret;
	}
};


template <typename T> class ForgetOctVarCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_FORGETOCTVAR; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		T infinity = AbstractCpuOperator<T>::implementation().infinity();
		plas::var_t v = plas::normalizeVar(args.var());

		ukoct_ASSERT(v > 0 && v <= state.octSize(), "Variable out of range, should be in between 1 and octSize.");

		plas::var_t min = 2 * v - 1;
		plas::var_t max = 2 * v;

		// Would be wrong if it wasn't know beforehand that each variable loop
		// will be run exactly twice.
		for (plas::var_t i = min; i <= max; ++i)
			for (plas::var_t j = min; j <= max; ++j)
				if (i == j)
					mat(i, j) = 0;
				else
					mat(i, j) = infinity;

		return ret;
	}
};


template <typename T> class CopyCpuOperator : public AbstractCpuOperator<T> {
public:
	ukoct::EOperation operation() const { return OPER_COPY; }

	CpuOperatorResult<T>* run(const OperatorArgs<T>& args, CpuOperatorResult<T>& ret) const {
		ukoct_ASSERT(args.other() != NULL, "Secondary matrix must be provided for this operator.");
		ukoct_ASSERT(AbstractCpuOperator<T>::implementation() != args.other()->implementation(), "Both matrices need to be from the same implementation.");
		ukoct::CpuState<T>& state = reinterpret_cast<CpuState<T>&>(args.state());
		ukoct::CpuState<T>& other = *reinterpret_cast<CpuState<T>*>(args.state());
		plas::DenseMatrix<T>& mat = state.input();
		plas::DenseMatrix<T>& oth = other.input();
		std::copy(oth.raw(), oth.raw() + (state.diffSize() * state.diffSize()), mat.raw());
		ret->boolResult(true);
		return ret;
	}
};


}
}
}


#endif /* UKOCT_CPU_OPERATORS_HPP_ */
