#ifndef UKOCT_CPU_HPP_
#define UKOCT_CPU_HPP_

#include <cmath>
#include <limits>
#include "ukoct/core.hpp"

namespace ukoct {

template <typename T> class OctDbm<T, ukoct::cpu> : public IOctDbm<T> {
public:
	typedef T Type;
	typedef typename ukoct::cpu Backend;
	typedef OctDbm<T, ukoct::cpu> DbmType;
	typedef std::numeric_limits<T> TypeLimits;

	OctDbm() :
		_valid(false),
		_mat(_selfMat)
	{
		_mat.defaultVal(TypeLimits::has_infinity ? TypeLimits::infinity() : TypeLimits::max());
	}


	OctDbm(const DbmType& other) :
		_valid(other._valid),
		_selfMat(other._selfMat),
		_mat(other._mat) { checkValidity(); }


	OctDbm(const plas::IMatrix<T>& mat) :
		_valid(false),
		_mat(_selfMat) { mat.copyto(_mat); checkValidity(); }


	OctDbm(plas::IMatrix<T>& mat) :
		_valid(false),
		_mat(mat) { checkValidity(); }


	OctDbm(const plas::DenseMatrix<T>& mat) :
		_valid(false),
		_selfMat(mat),
		_mat(_selfMat) { checkValidity(); }


	OctDbm(plas::DenseMatrix<T>& mat) :
		_valid(false),
		_mat(mat) { checkValidity(); }


	bool valid() {
		return _valid;
	}


	inline size_t size() {
		return _mat.colsize();
	}


	inline size_t octSize() {
		return size() / 2;
	}


	inline T hugeval() {
		return _mat.defaultVal();
	}


	bool consistent() {
		for (plas::var_t k = 1; k <= size(); ++k) {
			if (_mat(k, k) < 0)
				return false;
			else
				_mat(k, k) = 0;
		}
		return true;
	}


	bool intConsistent() {
		for (plas::var_t k = 1; k <= size(); ++k) {
			plas::var_t K = plas::switchVar(k);
			if (_mat(k, K) + _mat(K, k) < 0)
				return false;
		}
		return true;
	}


	bool coherent() {
		for (plas::var_t i = 1; i <= size(); ++i) {
			for (plas::var_t j = 1; i <= size(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				if (_mat(i, j) != _mat(J, I))
					return false;
			}
		}

		return true;
	}


	bool closed() {
		if (!consistent()) return false;

		for (plas::var_t i = 1; i <= size(); ++i) {
			for (plas::var_t j = 1; j <= size(); ++j)
				for (plas::var_t k = 1; k <= size(); ++k)
					if (_mat(i, j) > _mat(i, k) + _mat(k, j))
						return false;
		}

		return true;
	}


	bool stronglyClosed() {
		if (!consistent()) return false;

		for (plas::var_t i = 1; i <= size(); ++i) {
			for (plas::var_t j = 1; j <= size(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				if (_mat(i, j) > (_mat(i, I) + _mat(J, j)) / 2)
					return false;

				for (plas::var_t k = 1; k <= size(); ++k)
					if (_mat(i, j) > _mat(i, k) + _mat(k, j))
						return false;
			}
		}
		return true;
	}


	bool tightlyClosed() {
		for (plas::var_t k = 1; k <= size(); ++k) {
			plas::var_t K = plas::switchVar(k);

			// for all i, m(i, i) = 0
			if (_mat(k, k) < 0)
				return false;
			else
				_mat(k, k) = 0;

			// for all i, m(i, I) is even
			if (_mat(k, K) & 1) // _mat(i, I) % 2 != 0
				return false;
		}

		for (plas::var_t i = 1; i <= size(); ++i) {
			for (plas::var_t j = 1; j <= size(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				// for all i j, m[ij] <= (m[iI] + m[Jj]) / 2
				if (_mat(i, j) > (_mat(i, I) + _mat(J, j)) / 2)
					return false;

				for (plas::var_t k = 1; k <= size(); ++k)
					if (_mat(i, j) > _mat(i, k) + _mat(k, j))
						return false;
			}
		}

		return true;
	}


	bool weaklyClosed() {
		for (plas::var_t k = 1; k <= size(); ++k) {
			if (_mat(k, k) < 0)
				return false;
			else
				_mat(k, k) = 0;
		}

		for (plas::var_t i = 1; i <= size(); ++i) {
			for (plas::var_t j = 1; j <= size(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);

				for (plas::var_t k = 1; k <= size(); ++k) {
					if (_mat(i, k) + _mat(k, j) < std::min(_mat(i, j), (_mat(i, I) + _mat(J, j)) / 2))
						return false;
				}
			}
		}

		return true;
	}


	bool isTop() {
		for (plas::var_t i = 1; i <= size(); ++i)
			for (plas::var_t j = 1; j <= size(); ++j)
				if (i != j && _mat(i, j) != hugeval())
					return false;
		return true;
	}


	bool closure() {
		shortestPath();
		if (!consistent())
			return false;
		//strengthen();
		return true;
	}


	bool tightClosure() {
		shortestPath();
		if (!consistent())
			return false;
		tighten();
		if (!intConsistent())
			return false;
		return true;
	}


	DbmType& shortestPath(size_t iters = 0) {
		if (iters == 0)
			iters = size();

		if (iters > size() / 2)
			iters = size() / 2;

		for (size_t k = 1; k <= size(); ++k, ++k) {
			plas::var_t K = plas::switchVar(k);

			for (plas::var_t i = 1; i <= size(); ++i)
				for (plas::var_t j = 1; j <= size(); ++j) {
					plas::var_t I = plas::switchVar(i);
					plas::var_t J = plas::switchVar(j);
					_mat(i, j) = std::min(
						_mat(i, j), std::min(                              // i -> j
							_mat(i, k) + _mat(k, j), std::min(             // i -> k -> j
								_mat(i, K) + _mat(K, j), std::min(         // i -> K -> j
									_mat(i, k) + _mat(k, K) + _mat(K, j),  // i -> k -> K -> j
									_mat(i, K) + _mat(K, k) + _mat(k, j)   // i -> K -> k -> j
								)
							)
						)
					);
				}

			// QUESTION: Is it needed to strengthen right after shortest path, as in miné's paper,
			// or can it be left to closure?
			//strengthen();
		}
		return *this;
	}


	DbmType& strengthen(bool intBased = false) {
		for (plas::var_t i = 1; i <= size(); ++i)
			for (plas::var_t j = 1; j <= size(); ++j) {
				plas::var_t I = plas::switchVar(i);
				plas::var_t J = plas::switchVar(j);
				if (!intBased)
					_mat(i, j) = std::min(_mat(i, j), (_mat(i, I) + _mat(J, j)) / 2);

				else {
					if (i == j)
						_mat(i, j) = std::min(_mat(i, j), std::floor((_mat(i, I) + _mat(J, j)) / 2));
					else
						_mat(i, j) = 2 * std::floor(_mat(i, I) / 2);
				}
			}
		return *this;
	}


	DbmType& tighten() {
		for (plas::var_t i = 1; i <= size(); ++i) {
			plas::var_t I = plas::switchVar(i);
			_mat(i, I) = 2 * std::floor(_mat(i, I) / 2);
		}
		return *this;
	}


	DbmType& top() {
		for (plas::var_t i = 1; i <= size(); ++i)
			for (plas::var_t j = 1; j <= size(); ++j)
				if (i == j)
					_mat(i, j) = 0;
				else
					_mat(i, j) = hugeval();

		return *this;
	}


	bool includes(const OctDbm<T, cpu>& other) {
		if (this == &other)
			return false;

		if (size() != other.nvars())
			throw Error("Problem sizes cannot be different.");

		for (plas::var_t i = 1; i <= size(); ++i)
			for (plas::var_t j = 1; j <= size(); ++j)
				if (_mat(i, j) > other._mat(i, j))
					return false;

		return true;
	}


	int compare(const OctDbm<T, cpu>& other) {
		throw Error("Not implemented.");
	}


	DbmType& operator<<(plas::OctDiffConstraint<T>& f) {
		for (plas::var_t i = 1; i < size(); ++i) {
			for (plas::var_t j = 1; j < size(); ++j) {
				_mat(i, j) = std::min(_mat(i, j), _mat(i, f.a()) + f.d() + _mat(f.b(), j));
			}
		}
		return *this;
	}


	DbmType& operator<<(plas::OctConstraint<T>& f) {
		plas::OctDiffConstraint<T> ca, cb;
		f.split(ca, cb);
		for (plas::var_t i = 1; i < size(); ++i) {
			for (plas::var_t j = 1; j < size(); ++j) {
				T da = _mat(i, ca.a()) + ca.d() + _mat(ca.b(), j);
				T db = da;
				if (cb.valid())
					db = _mat(i, cb.a()) + cb.d() + _mat(cb.b(), j);
				_mat(i, j) = std::min(_mat(i, j), std::min(da, db));
			}
		}
		return *this;
	}


	DbmType& operator>>(plas::var_t v) {
		v = plas::normalizeVar(v);

		if (v <= 0 || v > (size() / 2))
			throw Error("Variable out of range, should be in between 1 and nvars / 2.");

		plas::var_t min = 2 * v - 1;
		plas::var_t max = 2 * v;

		// Would be wrong if it wasn't know beforehand that each variable loop
		// will be run exactly twice.
		for (plas::var_t i = min; i <= max; ++i)
			for (plas::var_t j = min; j <= max; ++j)
				if (i == j)
					_mat(i, j) = 0;
				else
					_mat(i, j) = hugeval();
		return *this;
	}


	DbmType& operator=(const DbmType& rhs) {
		if (this != &rhs) {
			_valid = rhs._valid;
			_mat = rhs._valid;
			_selfMat = rhs._selfMat;
		}
		return *this;
	}


	bool operator==(const DbmType& other) {
		if (this == &other)
			return true;

		if (size() != other.numOctVars())
			return false;

		for (plas::var_t i = 1; i <= size(); ++i)
			for (plas::var_t j = 1; j <= size(); ++j)
				if (_mat(i, j) != other._mat(i, j))
					return false;
		return true;
	}


	bool operator!=(const DbmType& other) {
		return !this->operator==(other);
	}


	// if true, this < other means that this is a subset of other
	bool operator<(const OctDbm<T, cpu>& other) {
		return other.includes(*this);
	}


	DbmType operator&(const DbmType& other) {
		DbmType ret(*this);
		ret &= other;
		return ret;
	}


	DbmType operator|(const DbmType& other) {
		DbmType ret(*this);
		ret |= other;
		return ret;
	}


	DbmType& operator&=(const DbmType& other) {
		if (this == &other)
			return *this;
		if (size() != other.numOctVars())
			return *this; // Maybe raise an error?
		for (plas::var_t i = 1; i <= size(); ++i)
			for (plas::var_t j = 1; j <= size(); ++j)
				_mat(i, j) = std::max(_mat(i, j), other._mat(i, j));
		return *this;
	}


	DbmType& operator|=(const DbmType& other) {
		if (this == &other)
			return *this;
		if (size() != other.numOctVars())
			return *this; // Maybe raise an error?
		for (plas::var_t i = 1; i <= size(); ++i)
			for (plas::var_t j = 1; j <= size(); ++j)
				_mat(i, j) = std::min(_mat(i, j), other._mat(i, j));
		return *this;
	}



protected:
	void checkValidity() {
		_valid = true;

		if (_mat.colsize() != _mat.rowsize())
			//throw error("The input matrix must be square.");
			_valid = false;

		if (_mat.colsize() % 2 != 0)
			// throw error("The input (square) matrix's size must be a multiple of two.");
			_valid = false;

		if (_mat.colsize() < 2)
			// throw error("The input (square) matrix's size must be at least 2.");
			_valid = false;
	}

private:
	bool _valid;
	plas::IMatrix<T>& _mat;
	plas::DenseMatrix<T> _selfMat;
};


template <typename T> class CpuOctDbm : public OctDbm<T, ukoct::cpu> {
	typedef T Type;
	typedef typename OctDbm<T, ukoct::cpu>::Backend Backend;
	typedef OctDbm<T, ukoct::cpu> DbmType;
	typedef std::numeric_limits<T> TypeLimits;
};

struct cpu {};

};

#endif /* UKOCT_CPU_HPP_ */
