#ifndef PLAS_HPP_
#define PLAS_HPP_

#include <limits>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>

#ifndef plas_VARTYPE
# 	define plas_VARTYPE int
#endif

#ifndef plas_UVARTYPE
#	define plas_UVARTYPE unsigned plas_VARTYPE
#endif

namespace plas {

//static const char _assert_vartypesize_le_sizet[sizeof(size_t) - sizeof(plas_VARTYPE) - 1] = {};

/*
 * [ [ [  ENUMS AND DEFINITIONS ] ] ]
 */


enum EMatrixOrdering {
	MATRIX_NOTAPPLICABLE,
	MATRIX_ROWMAJOR,
	MATRIX_COLMAJOR,
	MATRIX_DEFAULTMAJOR = MATRIX_ROWMAJOR
};


enum EProblemType {
	PROBLEM_NONE,
	PROBLEM_OCTDIFF,
	PROBLEM_OCT,
	PROBLEM_GRAPH
};


enum EProblemNature {
	PROBLEM_REAL,
	PROBLEM_INT
};


typedef plas_VARTYPE var_t;
typedef plas_UVARTYPE uvar_t;
typedef typename std::numeric_limits<var_t> var_limits;
typedef typename std::numeric_limits<uvar_t> uvar_limits;
static const var_t INVALID_VAR = 0;


/*
 * [ [ [ INTERFACES ] ] ]
 */


template <typename T> struct IMatrix {
	virtual ~IMatrix() {}
	virtual T& operator()(var_t row, var_t col = 0) = 0;
	virtual T* raw() = 0; // Returns pointer to first element, or NULL if matrix is sparse.
	virtual IMatrix& copyto(IMatrix& to) = 0;
	virtual var_t colsize() const = 0;
	virtual var_t rowsize() const = 0;
	virtual bool resize(var_t rowsize, var_t colsize) = 0;
	virtual IMatrix& reset() = 0;
	virtual EMatrixOrdering ordering() const = 0;
	virtual T defaultVal() const = 0;
	virtual IMatrix& defaultVal(T val) = 0;
};


/*
 * [ [ [ AUXILIARY FUNCTIONS ] ] ]
 */


static inline var_t normalizeVar(var_t var) {
	return var < 0 ? -var : var;
}


static inline bool isValidVar(var_t var)
	{ return var != INVALID_VAR; }


static inline var_t switchVar(var_t var) {
	var = normalizeVar(var);
	return var == INVALID_VAR
		? INVALID_VAR
		: ( (var & 1) ^ 1 // var % 2 == 0
			? var - 1
			: var + 1
		);
}


static inline bool isFromPositiveVar(var_t normalizedVar) {
	normalizedVar = normalizeVar(normalizedVar);
	return (normalizedVar & 1) ^ 1 ? false : true;
}


static inline std::string generateTransformedVarName(const std::string& base, var_t var) {
	return std::string(base) + (isFromPositiveVar(var) ? "__pos" : "__neg");
}


static inline var_t generateTransformedVar(var_t var) {
	return var == 0
		? 0
		: var < 0
			? normalizeVar(var + 1) * 2 + 2
			: (var - 1) * 2 + 1;
}


static inline var_t generateOriginalVar(var_t normalizedVar) {
	normalizedVar = normalizeVar(normalizedVar);
	return normalizedVar == INVALID_VAR
		? INVALID_VAR
		: isFromPositiveVar(normalizedVar)
			? normalizedVar / 2 + 1
			: (normalizedVar - 1) / 2 + 1;
}


#if 0
static inline size_t generateMatrixVar(var_t var) {
	var = normalizeVar(var);
	return var == INVALID_VAR ? (size_t) -1 : var - 1;
}
#endif


template <typename T>
static inline size_t generateMatrixIndex(const IMatrix<T>& ctnr, var_t row, var_t col) {
	row--; col--;
	switch (ctnr.ordering()) {
	case MATRIX_ROWMAJOR: return (row * ctnr.colsize()) + col; break;
	case MATRIX_COLMAJOR: return  row + col * ctnr.colsize(); break;
	default: return 0;
	}
}


/*
 * [ [ [ CLASSES ] ] ]
 */


template <typename T> struct OctDiffConstraint {
	OctDiffConstraint() :
		_a(INVALID_VAR),
		_b(INVALID_VAR),
		_d(T()) {}
	OctDiffConstraint(const OctDiffConstraint<T>& rhs) :
		_a(rhs._a),
		_b(rhs._b),
		_d(rhs._d) {}
	OctDiffConstraint(var_t a, var_t b, T d = T()) :
		_a(a),
		_b(b),
		_d(d) {}
	inline T d() const
		{ return _d; }
	inline OctDiffConstraint& d(T d)
		{ _d = d; return *this; }
	inline T constant() const
		{ return _d; }
	inline OctDiffConstraint& constant(T d)
		{ _d = d; return *this; }
	inline var_t a() const
		{ return _a; }
	inline var_t A() const
		{ return switchVar(_a); }
	inline OctDiffConstraint& a(var_t a)
		{ _a = a; return *this; }
	inline var_t b() const
		{ return _b; }
	inline var_t B() const
		{ return switchVar(_b); }
	inline OctDiffConstraint& b(var_t b)
		{ _b = b; return *this; }
	inline bool isSingleVar() { return a() == b() && a() != INVALID_VAR; }
	inline bool valid() { return a() != INVALID_VAR && b() != INVALID_VAR; }
	inline void invalidade() { _a = INVALID_VAR; _b = INVALID_VAR; }
private:
	var_t _a, _b;
	T _d;
};


template <typename T> struct OctConstraint : public OctDiffConstraint<T> {
public:
	OctConstraint() : OctDiffConstraint<T>() {}
	OctConstraint(const OctConstraint<T>& rhs) : OctDiffConstraint<T>(rhs) {}
	OctConstraint(const OctDiffConstraint<T>& rhs) : OctDiffConstraint<T>(rhs) {}
	OctConstraint(var_t a, var_t b, T d = T()) : OctDiffConstraint<T>(a, b, d) {}

	void split(OctDiffConstraint<T>& ca, OctDiffConstraint<T>& cb) {
		ca.invalidade(); cb.invalidade();

		if (OctDiffConstraint<T>::isSingleVar()) {
			var_t var = OctDiffConstraint<T>::a();
			var_t newVar = generateTransformedVar(var);
			var_t posVar = var < 0 ? newVar - 1 : newVar;
			var_t negVar = var < 0 ? newVar : newVar + 1;
			OctDiffConstraint<T>& c = ca;

			c.a(newVar);
			if (var < 0)
				c.b(newVar - 1);
			else
				c.b(newVar + 1);
			c.constant(2 * OctDiffConstraint<T>::constant());

		} else {
			var_t a = OctDiffConstraint<T>::a();
			var_t b = OctDiffConstraint<T>::b();

			if (a < 0 && b > 0) {
				var_t t = a; a = b; b = a;
			}

			var_t newa = generateTransformedVar(a);
			var_t newb = generateTransformedVar(b);
			var_t posa = a < 0 ? newa - 1 : newa ;
			var_t nega = a < 0 ? newa : newa + 1;
			var_t posb = b < 0 ? newb - 1 : newb;
			var_t negb = b < 0 ? newb : newb + 1;
			ca.constant(OctDiffConstraint<T>::constant());
			cb.constant(OctDiffConstraint<T>::constant());

			if (a > 0 && b > 0) {
				ca.a(posa);
				ca.b(negb);
				cb.a(posb);
				cb.b(nega);

			} else if (a > 0 && b < 0) {
				ca.a(posa);
				ca.b(posb);
				cb.a(negb);
				cb.b(nega);

			} else if (a < 0 && b < 0) {
				ca.a(nega);
				ca.b(posb);
				cb.a(negb);
				cb.b(posa);

			} else {
				// One of them is zero!
			}
		}
	}

	void split(OctDiffConstraint<T>* ret) {
		split(ret[0], ret[1]);
	}
};


template <class T> class NullMatrix : public IMatrix<T> {
public:
	typedef std::vector<T> ContainerType;
	typedef typename ContainerType::iterator Iterator;
	T& operator()(var_t row, var_t col = 0) { return _val; }
	T* raw() { return NULL; }
	NullMatrix& copyto(IMatrix<T>& to) { to.reset(); return *this; }
	var_t colsize() const { return 0; }
	var_t rowsize() const { return 0; }
	bool resize(var_t rowsize, var_t colsize) { return true; }
	NullMatrix& reset() { return *this; }
	EMatrixOrdering ordering() const { return MATRIX_NOTAPPLICABLE; }
	T defaultVal() const { return T(); }
	NullMatrix& defaultVal(T val) { return *this; }

private:
	T _val;
};


template <class T> class DenseMatrix : public IMatrix<T> {
public:
	typedef std::vector<T> ContainerType;
	typedef typename ContainerType::iterator Iterator;
	typedef typename std::numeric_limits<T> TypeLimits;

	DenseMatrix() :
		_order(MATRIX_DEFAULTMAJOR),
		_rawdata(NULL),
		_colsize(0),
		_rowsize(0),
		_defaultVal(TypeLimits::is_specialized ? (TypeLimits::has_infinity ? TypeLimits::infinity() : TypeLimits::max()) : T()) {}
	DenseMatrix(const DenseMatrix& other) :
		_order(other._order),
		_rawdata(other._rawdata),
		_data(other._data),
		_colsize(other._colsize),
		_rowsize(other._rowsize),
		_defaultVal(other._defaultVal) {}
	DenseMatrix(var_t rowsize, var_t colsize, EMatrixOrdering order = MATRIX_DEFAULTMAJOR, T defaultVal = TypeLimits::is_specialized ? (TypeLimits::has_infinity ? TypeLimits::infinity() : TypeLimits::max()) : T()) :
		_order(order),
		_rawdata(NULL),
		_colsize(0),
		_rowsize(0),
		_defaultVal(defaultVal)
		{ resize(colsize, rowsize); }
	DenseMatrix(T* data, var_t nrows, var_t ncols, EMatrixOrdering order = MATRIX_DEFAULTMAJOR) :
		_order(order),
		_rawdata(data),
		_colsize(ncols),
		_rowsize(nrows),
		_defaultVal(TypeLimits::is_specialized ? (TypeLimits::has_infinity ? TypeLimits::infinity() : TypeLimits::max()) : T()) {}
	T& operator()(var_t row, var_t col) {
		//row++; col++;
		size_t idx = generateMatrixIndex(*this, row, col);
		if (_rawdata == NULL && (col > _colsize || row > _rowsize)) {
			var_t newcolsize = col > _colsize ? col : _colsize;
			var_t newrowsize = row > _rowsize ? row : _rowsize;
			DenseMatrix<T> copy(const_cast<DenseMatrix&>(*this));

			if (resize(newrowsize, newcolsize)) {
				for (var_t r = 1; r <= copy.colsize(); ++r) {
					for (var_t c = 1; c <= copy.rowsize(); ++c) {
						size_t i = generateMatrixIndex(*this, r, c);
						_data[i] = copy(r, c);
					}
				}
			}
		}
		return _rawdata != NULL ? _rawdata[idx] : _data[idx];
	}
	T* raw()
		{ return _rawdata != NULL ? _rawdata : &_data[0]; }
	DenseMatrix& copyto(IMatrix<T>& to) {
		for (var_t r = 1; r <= _colsize; ++r)
			for (var_t c = 1; c <= _rowsize; ++c)
				to(r, c) = this->operator()(r, c);
		return *this;
	}
	var_t colsize() const
		{ return _colsize; }
	var_t rowsize() const
		{ return _rowsize; }
	bool resize(var_t rowsize, var_t colsize) {
		if (_rawdata == NULL && (colsize != _colsize || rowsize != _rowsize)) {
			if (rowsize > 0 && colsize > 0) {
				_colsize = colsize;
				_rowsize = rowsize;
				_data.resize(colsize * rowsize, _defaultVal);
				return true;
			}
		}
		return false;
	}
	DenseMatrix& reset() {
		for (Iterator it = _data.begin(); it != _data.end(); ++it)
			*it = _defaultVal;
		return *this;
	}
	T defaultVal() const { return _defaultVal; }
	DenseMatrix& defaultVal(T val) { _defaultVal = val; return *this; }
	EMatrixOrdering ordering() const
		{ return _order; }

private:
	T _defaultVal;
	var_t _colsize;
	var_t _rowsize;
	std::vector<T> _data;
	EMatrixOrdering _order;
	T* _rawdata;
};


template <typename T, template <typename T> class M = DenseMatrix> struct Problem {
	typedef plas::OctDiffConstraint<T> OctConstraint;
	typedef M<T> DefaultMatrix;

	struct Entry {
		Entry() :
			_idx(0),
			_originalIdx(0),
			_fileline(0) {}
		Entry(const Entry& rhs) :
			_idx(rhs._idx),
			_originalIdx(rhs._originalIdx),
			_octConstraint(rhs._octConstraint),
			_fileline(rhs._fileline) {}
		Entry(size_t idx) :
			_idx(idx),
			_originalIdx(0),
			_fileline(0) {}
		size_t index() const
			{ return _idx; }
		Entry& index(size_t idx)
			{ _idx = idx; return *this; }
		size_t originalIndex() const
			{ return _originalIdx; }
		Entry& originalIndex(size_t idx)
			{ _originalIdx = idx; return *this; }
		size_t fileline() const
			{ return _fileline; }
		Entry& fileline(size_t fileline)
			{ _fileline = fileline; return *this; }
		OctConstraint& octConstraint()
			{ return _octConstraint; }
		Entry& octConstraint(OctConstraint octConstraint)
			{ _octConstraint = octConstraint; return *this; }
	private:
		size_t _idx;
		size_t _fileline;
		size_t _originalIdx;
		OctConstraint _octConstraint;
	};

	struct NameEntry {
		NameEntry() :
			_normalizedVar(0),
			_fileline(0) {}
		NameEntry(const NameEntry& rhs) :
			_name(rhs._name),
			_normalizedVar(rhs._normalizedVar),
			_fileline(rhs._fileline) {}
		NameEntry(var_t var) :
			_normalizedVar(normalizeVar(var)),
			_fileline(0) {}
		NameEntry(var_t var, const std::string& name, size_t fileline = 0) :
			_normalizedVar(normalizeVar(var)),
			_name(name),
			_fileline(fileline) {}
		NameEntry(var_t var, const char* name, size_t fileline = 0) :
			_normalizedVar(normalizeVar(var)),
			_name(name),
			_fileline(fileline) {}
		var_t& normalizedVar() { return _normalizedVar; }
		NameEntry& normalizedVar(var_t var) { _normalizedVar = normalizeVar(var); return *this; }
		std::string& name() { return _name; }
		NameEntry& name(const std::string& name) { _name = name; return *this; }
		NameEntry& name(const char* name) { _name = name; return *this; }
		size_t& fileline() { return _fileline; }
		NameEntry& fileline(size_t fileline) { _fileline = fileline; return *this; }

	private:
		var_t _normalizedVar;
		std::string _name;
		size_t _fileline;
	};

	// List containing all entries
	typedef typename std::vector<Entry> EntryList;
	typedef typename EntryList::iterator EntryListIterator;
	// Map relating variable appearance with entries
	typedef typename std::map<var_t, Entry> EntryMap;
	typedef typename EntryMap::iterator EntryMapIterator;
	// Map holding declared variable names
	typedef typename std::map<var_t, NameEntry> NamesMap;
	typedef typename NamesMap::iterator NamesMapIterator;
	// Set containing all declared variables
	typedef typename std::set<var_t> VarSet;
	typedef typename VarSet::iterator VarSetIterator;

	Problem() :
		_matrix(&_selfMatrix),
		_problemType(PROBLEM_NONE),
		_problemNature(PROBLEM_REAL),
		_minConstant(std::numeric_limits<T>::max()),
		_maxConstant(std::numeric_limits<T>::min()),
		_vars(&_selfVars),
		_numVars(0),
		_declaredNumVars(0),
		_declaredNumEntries(0),
		_declaredNumConstraints(0),
		_entries(),
		_names(&_selfNames) {}

	Problem(IMatrix<T>& matrix) :
		_matrix(&matrix),
		_problemType(PROBLEM_NONE),
		_problemNature(PROBLEM_REAL),
		_minConstant(std::numeric_limits<T>::max()),
		_maxConstant(std::numeric_limits<T>::min()),
		_vars(&_selfVars),
		_numVars(0),
		_declaredNumVars(0),
		_declaredNumEntries(0),
		_declaredNumConstraints(0),
		_entries(),
		_names(&_selfNames) {}

	Problem(M<T>& matrix) :
		_matrix(&matrix),
		_problemType(PROBLEM_NONE),
		_problemNature(PROBLEM_REAL),
		_minConstant(std::numeric_limits<T>::max()),
		_maxConstant(std::numeric_limits<T>::min()),
		_vars(&_selfVars),
		_numVars(0),
		_declaredNumVars(0),
		_declaredNumEntries(0),
		_declaredNumConstraints(0),
		_entries(),
		_names(&_selfNames) {}

	Problem(const Problem& rhs) :
		_matrix(rhs._matrix),
		_problemType(rhs._problemType),
		_problemNature(rhs._problemNature),
		_minConstant(rhs._minConstant),
		_maxConstant(rhs._maxConstant),
		_vars(rhs._vars),
		_numVars(rhs._numVars),
		_declaredNumVars(rhs._declaredNumVars),
		_declaredNumEntries(rhs._declaredNumEntries),
		_declaredNumConstraints(rhs._declaredNumConstraints),
		_entries(rhs._entries),
		_names(rhs._names) {}

	EProblemType problemType() const { return _problemType; }
	EProblemNature problemNature() const { return _problemNature; }
	Problem& problemType(EProblemType problemType) { _problemType = problemType; return *this; }
	Problem& problemNature(EProblemNature problemNature) { _problemNature = problemNature; return *this; }
	size_t numVars() const { return _numVars; }
	size_t declaredNumVars() const { return _declaredNumVars; }
	size_t declaredNumEntries() const { return _declaredNumEntries; }
	size_t declaredNumConstraints() const { return _declaredNumConstraints; }
	Problem& numVars(size_t numVars) { _numVars = numVars; return *this; }
	Problem& declaredNumVars(size_t numVars) { _declaredNumVars = numVars; return *this; }
	Problem& declaredNumEntries(size_t numEntries) { _declaredNumEntries = numEntries; return *this; }
	Problem& declaredNumConstraints(size_t numConstraints) { _declaredNumConstraints = numConstraints; return *this; }
	EntryList& entries() { return _entries; }
	Entry& entry(size_t idx) { return entries()[idx]; }
	NamesMap* names() const { return _names; }
	Problem& names(NamesMap* names) { _names = names; return *this; }
	VarSet* vars() const { return _vars; }
	Problem& vars(VarSet* vars) { _vars = vars; return *this; }
	T minConstant() const { return _minConstant; }
	T maxConstant() const { return _maxConstant; }
	Problem& minConstant(T minConstant) { if (minConstant < _minConstant) _minConstant = minConstant; return *this; }
	Problem& maxConstant(T maxConstant) { if (maxConstant > _maxConstant) _maxConstant = maxConstant; return *this; }
	const std::string& filename() const { return _filename; }
	Problem& filename(const std::string& filename) { _filename = filename; return *this; }
	Problem& filename(const char* filename) { _filename = filename; return *this; }
	IMatrix<T>* matrix() const { return _matrix; }
	Problem& matrix(IMatrix<T>* matrix) { _matrix = matrix; return *this; }
	Problem& reset() {
		_problemType = PROBLEM_NONE;
		_problemNature = PROBLEM_REAL;
		_minConstant = std::numeric_limits<T>::max();
		_maxConstant = std::numeric_limits<T>::min();
		_numVars = 0;
		_declaredNumVars = 0;
		_declaredNumEntries = 0;
		_declaredNumConstraints = 0;
		_entries.clear();
		if (_matrix != NULL) _matrix->reset();
		if (_names != NULL) _names->clear();
		if (_vars != NULL) _vars->clear();
		return *this;
	}
	std::string generateVarName(var_t var, bool forceSign = false) {
		std::ostringstream s;
		if (forceSign) s.setf(std::ios_base::showpos);
		if (_names != NULL) {
			var_t normalizedVar = normalizeVar(var);
			NamesMapIterator it = _names->find(normalizedVar);

			if (var != plas::INVALID_VAR && it != _names->end()) {
				if (var < 0)
					s << "-";
				else if (forceSign && var > 0)
					s << "+";
				s << it->second.name();
			} else
				s << var;
		} else
			s << var;
		return std::string(s.str());
	}
	std::string generateConstraintText(EntryListIterator iter) {
		std::ostringstream s;
		if (iter != _entries.end()) {
			Entry& entry = *iter;
			T constant = entry.octConstraint().constant();
			var_t b = entry.octConstraint().b();
			var_t a = entry.octConstraint().a();
			if (_matrix != NULL && b > 0 && a > 0)// && _problemType == PROBLEM_OCTDIFF )
				constant = _matrix->operator()(a, b);
			if (entry.octConstraint().isSingleVar()) s
				<< generateVarName(entry.octConstraint().a())
				<< " <= " << constant;
			else s
				<< generateVarName(entry.octConstraint().a())
				<< (_problemType == PROBLEM_OCTDIFF ? " -" : " ") << generateVarName(entry.octConstraint().b(), _problemType != PROBLEM_OCTDIFF)
				<< " <= " << constant;
		}
		return std::string(s.str());
	}
	std::string generateConstraintText(size_t idx) {
		EntryListIterator iter = idx < _entries.size() ? _entries.begin(idx) + idx : _entries.end();
		return generateConstraintText(iter);
	}

private:
	std::string _filename;
	EProblemType _problemType;
	EProblemNature _problemNature;
	T _minConstant;
	T _maxConstant;
	size_t _numVars;
	size_t _declaredNumVars;
	size_t _declaredNumEntries;
	size_t _declaredNumConstraints;
	EntryList _entries;
	IMatrix<T>* _matrix;
	NamesMap* _names;
	VarSet* _vars;
	M<T> _selfMatrix;
	NamesMap _selfNames;
	VarSet _selfVars;
};


/*
 * [ [ [ FUNCTIONS ] ] ]
 */


template <typename T, template <typename T> class M>
static bool open(const std::string& name, Problem<T, M>& problem, std::ostream* ls = NULL) {
	return open(name.c_str(), problem, ls);
}


template <typename T, template <typename T> class M>
static bool open(const char* name, Problem<T, M>& problem, std::ostream* ls = NULL) {
	bool result = false;
	std::ifstream is(name);
	if (is.is_open()) {
		result = open(is, problem, ls);
		problem.filename(name);
		is.close();
	}
	return result;
}


template <typename T, template <typename T> class M>
static bool open(std::istream& is, Problem<T, M>& problem, std::ostream* ls = NULL) {
	size_t linecount = 0;
	var_t maxNormalizedVar = 0;
	problem.reset();

	do {
		std::string line;
		std::getline(is, line);
		line.erase(0, line.find_first_not_of(" \n\r\t"));
		linecount++;

		// ignores comments or empty linesline[0] == 'n'
		std::cout << line  << std::endl;
		if (!line.empty() && line[0] != '#') {
			std::cout << line  << std::endl;
			char firstChar = line[0];
			bool isN = (firstChar == 'n');
			if (isN && problem.problemType() != PROBLEM_NONE && problem.names() != NULL) {
				typename Problem<T, M>::NamesMap& names = *problem.names();
				std::stringstream ss(line);
				var_t var;
				std::string name;
				ss >> name;
				name.clear(); // Remove 'n'
				ss >> var;
				ss >> name;
				if ((ss.good() || ss.eof())) {
					// TODO: Warnings about variables with values above the declared variable names
					// TODO: Warnings about negative variables
					typename Problem<T, M>::NameEntry nameEntry;
					nameEntry.normalizedVar(var).name(name).fileline(linecount);
					names[var] = nameEntry;
				}

			} else if (line[0] == 'e' && problem.problemType() == PROBLEM_GRAPH
					&& (problem.declaredNumEntries() == 0 || problem.entries().size() < problem.declaredNumEntries())) {
				// TODO: Revise this part
#if 0
				std::stringstream ss(line);
				size_t fromNodeIdx;
				size_t toNodeIdx;
				T cost;

				ss >> fromNodeIdx;
				if ((ss.good() || ss.eof()) && fromNodeIdx < problem.numVars()) {

					while (ss.good()) {
						ss >> toNodeIdx;
						ss >> cost;
						if ((ss.good() || ss.eof()) && toNodeIdx < problem.numVars()) {
							problem(fromNodeIdx, toNodeIdx) = cost;
							problem.minConstant(cost).maxConstant(cost);
						}
					}
				}
#endif

			} else if (line[0] == 'e' && (problem.problemType() == PROBLEM_OCT || problem.problemType() == PROBLEM_OCTDIFF)) {
				std::stringstream ss(line);
				std::string s;
				var_t a, b;
				T d;
				ss >> s; // remove 'e'
				ss >> a;
				ss >> b;
				ss >> d;
				if (a == INVALID_VAR) a = b;
				if (b == INVALID_VAR) b = a;

				if ((ss.good() || ss.eof())) {
					var_t na = normalizeVar(a);
					var_t nb = normalizeVar(b);
					typename Problem<T, M>::OctConstraint constraint(a, b, d);
					typename Problem<T, M>::Entry entry(problem.entries().size() + 1);
					entry.octConstraint(constraint).fileline(linecount);

					// Add to the
					problem.entries().push_back(entry);
					if (a != INVALID_VAR) {
						if (problem.vars() != NULL) problem.vars()->insert(a);
						if (na > maxNormalizedVar) maxNormalizedVar = na;
					}
					if (b != INVALID_VAR) {
						if (problem.vars() != NULL) problem.vars()->insert(b);
						if (nb > maxNormalizedVar) maxNormalizedVar = nb;
					}
					problem.minConstant(d).maxConstant(d);
					if (problem.matrix() != NULL && b > 0 && a > 0) {
						(*problem.matrix())(a, b) = d;
					}
				}


			} else {
				// Identify problem and parameters
				if (line[0] == 'p') {
					std::stringstream ss(line);
					std::string problemName;
					ss >> problemName;
					problemName.clear(); // remove the 'p'
					ss >> problemName;

					if (problemName.compare("octdiff") == 0) {
						problem
							.problemType(PROBLEM_OCTDIFF)
							.problemNature(PROBLEM_REAL);

					} else if (problemName.compare("oct") == 0) {
						problem
							.problemType(PROBLEM_OCT)
							.problemNature(PROBLEM_REAL);

					} else if (problemName.compare("octdiff_int") == 0) {
						problem
							.problemType(PROBLEM_OCTDIFF)
							.problemNature(PROBLEM_INT);

					} else if (problemName.compare("oct_int") == 0) {
						problem
							.problemType(PROBLEM_OCT)
							.problemNature(PROBLEM_INT);
					}


					if (problem.problemType() != PROBLEM_NONE) {
						size_t numEntries = 0, numVariables = 0;
						ss >> numVariables;
						ss >> numEntries;
						problem
							.declaredNumEntries(numEntries)
							.declaredNumVars(numVariables)
							.numVars(numVariables);
					}

					if (ss.good() || ss.eof()) {
						if (problem.matrix() != NULL) problem.matrix()->resize(
							problem.declaredNumVars(),
							problem.declaredNumVars());
					} else {
						problem.reset();
					}
				}
			}
		}
	} while (is.good());

	problem.numVars(maxNormalizedVar);

	return problem.problemType() != PROBLEM_NONE
			&& (problem.declaredNumEntries() == 0 || problem.entries().size() < problem.declaredNumEntries());
}


template <typename T, template <typename T> class M>
static bool toOctDiffProblem(Problem<T, M>& from, Problem<T, M>& to, std::ostream* ls = NULL) {
	size_t numVars = from.numVars() * 2;
	NullMatrix<T> nullMatrix;
	IMatrix<T>& matrix = to.matrix() != NULL ? *to.matrix() : nullMatrix;

	if (from.problemType() != PROBLEM_OCT)
		return false;

	to.reset();
	to.numVars(numVars);
	to.problemNature(from.problemNature());
	to.problemType(PROBLEM_OCTDIFF);

	if (!matrix.resize(numVars, numVars))
		return false;

	for (typename plas::Problem<T, M>::EntryListIterator it = from.entries().begin(); it != from.entries().end(); ++it) {
		if (it->octConstraint().isSingleVar()) {
			var_t var = it->octConstraint().a();
			var_t newVar = generateTransformedVar(var);
			var_t posVar = var < 0 ? newVar - 1 : newVar;
			var_t negVar = var < 0 ? newVar : newVar + 1;
			typename Problem<T, M>::Entry newEntry;

			newEntry.octConstraint().a(newVar);
			if (var < 0)
				newEntry.octConstraint().b(newVar - 1);
			else
				newEntry.octConstraint().b(newVar + 1);
			newEntry.octConstraint().constant(2 * it->octConstraint().constant());
			newEntry.originalIndex(it->index());
			newEntry.index(to.entries().size() + 1);

			if (to.vars() != NULL) {
				to.vars()->insert(posVar);
				to.vars()->insert(negVar);
			}

			if (to.names() != NULL && from.names() != NULL) {
				std::ostringstream posVarName, negVarName;
				typename Problem<T, M>::NameEntry posNameEntry, negNameEntry;

				posVarName << from.generateVarName(normalizeVar(var), false) << "__p" << normalizeVar(var) << "_" << posVar;
				negVarName << from.generateVarName(normalizeVar(var), false) << "__n" << normalizeVar(var) << "_" << negVar;
				posNameEntry.normalizedVar(posVar);
				negNameEntry.normalizedVar(negVar);
				posNameEntry.name(posVarName.str());
				negNameEntry.name(negVarName.str());
				to.names()->operator[](posVar) = posNameEntry;
				to.names()->operator[](negVar) = negNameEntry;
			}

			to.entries().push_back(newEntry);
			matrix(newEntry.octConstraint().a(), newEntry.octConstraint().b()) = newEntry.octConstraint().constant();
			std::cerr << "* CONVERTED ENTRY " << it->octConstraint() << " into " << newEntry.octConstraint() << std::endl;

		} else {
			var_t a = it->octConstraint().a();
			var_t b = it->octConstraint().b();

			if (a < 0 && b > 0) {
				var_t t = a; a = b; b = a;
			}

			var_t newa = generateTransformedVar(a);
			var_t newb = generateTransformedVar(b);
			var_t posa = a < 0 ? newa - 1 : newa ;
			var_t nega = a < 0 ? newa : newa + 1;
			var_t posb = b < 0 ? newb - 1 : newb;
			var_t negb = b < 0 ? newb : newb + 1;
			typename Problem<T, M>::Entry newEntryA, newEntryB;
			newEntryA.octConstraint().constant(it->octConstraint().constant());
			newEntryB.octConstraint().constant(it->octConstraint().constant());
			newEntryA.originalIndex(it->index());
			newEntryB.originalIndex(it->index());

			if (a > 0 && b > 0) {
				newEntryA.octConstraint().a(posa);
				newEntryA.octConstraint().b(negb);
				newEntryB.octConstraint().a(posb);
				newEntryB.octConstraint().b(nega);

			} else if (a > 0 && b < 0) {
				newEntryA.octConstraint().a(posa);
				newEntryA.octConstraint().b(posb);
				newEntryB.octConstraint().a(negb);
				newEntryB.octConstraint().b(nega);

			} else if (a < 0 && b < 0) {
				newEntryA.octConstraint().a(nega);
				newEntryA.octConstraint().b(posb);
				newEntryB.octConstraint().a(negb);
				newEntryB.octConstraint().b(posa);

			} else {
				// One of them is zero!
			}

			if (to.vars() != NULL) {
				to.vars()->insert(posa);
				to.vars()->insert(nega);
				to.vars()->insert(posb);
				to.vars()->insert(negb);
			}

			if (to.names() != NULL && from.names() != NULL) {
				std::ostringstream posaVarName, negaVarName, posbVarName, negbVarName;
				typename Problem<T, M>::NameEntry posaNameEntry, negaNameEntry, posbNameEntry, negbNameEntry;

				posaVarName << from.generateVarName(normalizeVar(a), false) << "__p" << normalizeVar(a) << "_" << posa;
				negaVarName << from.generateVarName(normalizeVar(a), false) << "__n" << normalizeVar(a) << "_" << nega;
				posbVarName << from.generateVarName(normalizeVar(b), false) << "__p" << normalizeVar(b) << "_" << posb;
				negbVarName << from.generateVarName(normalizeVar(b), false) << "__n" << normalizeVar(b) << "_" << negb;
				posaNameEntry.normalizedVar(posa);
				negaNameEntry.normalizedVar(nega);
				posbNameEntry.normalizedVar(posb);
				negbNameEntry.normalizedVar(negb);
				posaNameEntry.name(posaVarName.str());
				negaNameEntry.name(negaVarName.str());
				posbNameEntry.name(posbVarName.str());
				negbNameEntry.name(negbVarName.str());
				to.names()->operator[](posa) = posaNameEntry;
				to.names()->operator[](nega) = negaNameEntry;
				to.names()->operator[](posb) = posbNameEntry;
				to.names()->operator[](negb) = negbNameEntry;
			}

			to.entries().push_back(newEntryA);
			to.entries().push_back(newEntryB);
			matrix(newEntryA.octConstraint().a(), newEntryA.octConstraint().b()) = newEntryA.octConstraint().constant();
			matrix(newEntryB.octConstraint().a(), newEntryB.octConstraint().b()) = newEntryB.octConstraint().constant();

			std::cerr << "* CONVERTED ENTRY " << it->octConstraint() << " into " << newEntryA.octConstraint() << " and " << newEntryB.octConstraint() << std::endl;
		}
	}

	return true;
}

};

template <typename T>
std::ostream& operator<<(std::ostream& os, const plas::OctDiffConstraint<T>& oct) {
	os << oct.a() << " " << oct.b() << " <= " << oct.constant();
	return os;
}

template <typename T, template <typename T> class M>
std::ostream& operator<<(std::ostream& os, plas::Problem<T, M>& problem) {
	os << "PROBLEM " << problem.filename() << std::endl
		<< "\tproblemType: " << problem.problemType() << std::endl
		<< "\tproblemNature: " << problem.problemNature()<< std::endl
		<< "\tnumVars: " << problem.numVars() << std::endl
		<< "\tdeclaredNumVars: " << problem.declaredNumVars() << std::endl
		<< "\tdeclaredNumEntries: " << problem.declaredNumEntries() << std::endl
		<< "\tminConstant: " << problem.minConstant() << std::endl
		<< "\tmaxConstant: " << problem.maxConstant() << std::endl
		<< "\tEntries: " << std::endl;
	for (typename plas::Problem<T, M>::EntryListIterator it = problem.entries().begin(); it != problem.entries().end(); ++it) {
		os << "\t\t[" << it->index() << " @ line " << it->fileline() << ", from " << it->originalIndex() << "] ";
		os << problem.generateConstraintText(it) << std::endl;
	}
	if (problem.names() != NULL) {
		os << "\tNames: " << std::endl;
		for (typename plas::Problem<T, M>::NamesMapIterator it = problem.names()->begin(); it != problem.names()->end(); ++it) {
			os << "\t\t" << it->first << " @ line " << it->second.fileline() << " = " << it->second.name() << std::endl;
		}
	}
	return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, plas::IMatrix<T>& matrix) {
	for (size_t row = 0; row < matrix.rowsize(); ++row) {
		const char* sep = "";
		for (size_t col = 0; col < matrix.colsize(); ++col) {
			os << sep << matrix(col, row);
			sep = "\t";
		}
		os << std::endl;
	}
	return os;
}

#endif /* PLAS_HPP_ */
