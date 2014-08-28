#ifndef UKOCT_OPENCL_COMMON_POOL_HPP_
#define UKOCT_OPENCL_COMMON_POOL_HPP_


#include <algorithm>

namespace ukoct {
namespace impl {
namespace opencl {


/**
 * Basic implementation of an object pool.
 * Objects are held as values inside the pool, and therefore, unless the
 * type is not declared as being a reference, some copying may occur.
 */
template <
	typename T,
	class C = std::list<T>
>
struct ListPool {
	typedef typename C::value_type Type;
	typedef C Container;
	typedef typename C::iterator Iterator;
	typedef typename C::const_iterator ConstIterator;

	ListPool() : _selected(false) {}


	ListPool(const ListPool<C>& other) :
		_selected(other._selected),
		_selectedElem(other._selectedElem),
		_ctnr(other._ctnr) {}


	virtual ~ListPool() {}


	inline Iterator invalid() {
		return _ctnr.end();
	}


	inline bool hasSelected() {
		return _selected;
	}


	inline Type const& selected() {
		return _selectedElem;
	}


	inline void unselect() {
		if (_selected) {
			_ctnr.push_back(_selectedElem);
			_selectedElem = Type();
			_selected = false;
		}
	}


	inline Container& unselected() {
		return _ctnr;
	}


	inline Container all() {
		Container ret(_ctnr);
		ret.push_back(_selectedElem);
		return ret;
	}


	inline void select() {
		if (!_ctnr.empty())
			select(_ctnr.begin());
	}


	inline void select(const Iterator& it) {
		_selectedElem = *it;
		_selected = true;
		_ctnr.erase(it);
	}


	inline Iterator find(Type& item) {
		return std::find(_ctnr.begin(), _ctnr.end(), item);
	}


protected:
	inline virtual Iterator addNew(Type& item) {
		Iterator ret = _ctnr.insert(_ctnr.end(), item);
		return ret;
	}


public:
	bool _selected;
	Type _selectedElem;
	Container _ctnr;
};


template <
	typename K,
	typename V,
	class Compare = std::less<K>,
	typename C = std::map<K, V, Compare>
>
struct MapPool {
	typedef typename C::mapped_type MappedType;
	typedef typename C::key_type KeyType;
	typedef typename C::value_type ValueType;
	typedef C Container;
	typedef typename C::iterator Iterator;
	typedef typename C::const_iterator ConstIterator;


	/** Default constructor. */
	MapPool() : _selected(false) {}


	/** Copy constructor. */
	MapPool(const MapPool<K, V, Compare, C>& other) :
		_selected(other._selected),
		_selectedElem(other._selectedElem),
		_ctnr(other._ctnr) {}


	/** Destructor. */
	virtual ~MapPool() {}


	inline Iterator invalid() {
		return _ctnr.end();
	}


	inline bool hasSelected() {
		return _selected;
	}


	inline ValueType& selected() {
		return _selectedElem;
	}


	inline void unselect() {
		if (_selected) {
			_ctnr.insert(_selectedElem);
			_selectedElem = ValueType();
			_selected = false;
		}
	}

	inline Container& unselected() {
		return _ctnr;
	}


	inline Container all() {
		Container ret(_ctnr);
		ret.insert(_selectedElem);
		return ret;
	}


	inline void select() {
		if (!_ctnr.empty())
			select(_ctnr.first());
	}


	inline void select(const Iterator& it) {
		if (it != invalid()) {
			_selectedElem = *it;
			_selected = true;
			_ctnr.erase(it);
		}
	}


	inline void select(ValueType& it) {
		select(it.first);
	}


	inline void select(KeyType& v) {
		select(_ctnr.find(v));
	}


	inline Iterator find(ValueType& item) {
		return findKey(_ctnr.begin(), _ctnr.end(), item.first);
	}


	inline Iterator find(KeyType& item) {
		return findKey(_ctnr.begin(), _ctnr.end(), item);
	}


protected:
	virtual inline Iterator findKey(Iterator begin, Iterator end, KeyType& key) {
		while (begin != end) {
			if (compareKeys(key, begin->first) == 0)
				break;
			++begin;
		}
		return begin;
	}

	virtual inline int compareKeys(const KeyType& a, const KeyType& b) {
		typename C::key_compare compare = _ctnr.key_comp();
		return compare(a, b) - compare(b, a);
	}

	virtual inline Iterator addNew(ValueType& item) {
		Iterator ret = _ctnr.insert(_ctnr.end(), item);
		return ret;
	}


public:
	bool _selected;
	ValueType _selectedElem;
	Container _ctnr;
};


template <
	typename ST, // Selected type
	typename KT = ST, // Key type
	class Compare = std::less<KT>,
	class LC = std::list<ST>,
	class MC = std::map<KT, ST, Compare>
>
class MultiPool {
public:
	typedef MultiPool<ST, KT, Compare, LC, MC> SelfType;
	typedef KT ParamType;
	typedef ST SelectionType;
	typedef Compare CompareType;
	typedef LC SelectionList;
	typedef MC SelectionMap;
	typedef typename SelectionMap::iterator KeyIterator;
	typedef typename SelectionMap::const_iterator ConstKeyIterator;
	typedef typename SelectionList::iterator UnselectedIterator;
	typedef typename SelectionList::const_iterator ConstUnselectedIterator;


	MultiPool() {}


	MultiPool(const SelfType& other) :
		_selected(other._selected),
		_unselected(other._unselected) {}


	virtual ~MultiPool() {}


	inline KeyIterator invalid() {
		return _selected.end();
	}


	inline bool empty() {
		return !_selected.empty();
	}


	inline bool hasSelected(const ParamType& key) {
		return hasSelected(_selected.find(key));
	}


	inline bool hasSelected(const KeyIterator& it) {
		return it != _selected.end();
	}


	inline SelectionType& selected(const ParamType& key) {
		return selected(_selected.find(key));
	}


	inline SelectionType& selected(const KeyIterator& iter) {
		return iter->second;
	}


	inline const SelectionList& unselected() {
		return _unselected;
	}


	template <class Ctnr> inline Ctnr& keys(Ctnr& ctnr) {
		for (KeyIterator it = _selected.begin(); it != _selected.end(); ++it)
			ctnr.insert(ctnr.begin(), it->first);
		return ctnr;
	}


	template <class Ctnr> inline Ctnr& all(Ctnr& ctnr) {
		allSelected(ctnr);
		allUnselected(ctnr);
		return ctnr;
	}


	template <class Ctnr> inline Ctnr& allSelected(Ctnr& ctnr) {
		for (KeyIterator it = _selected.begin(); it != _selected.end(); ++it)
			if (it->second.isSelected())
				ctnr.insert(ctnr.end(), it->second.selected());
		return ctnr;
	}


	template <class Ctnr> inline Ctnr& allUnselected(Ctnr& ctnr) {
		for (KeyIterator mit = _selected.begin(); mit != _selected.end(); ++mit)
			for (UnselectedIterator it = mit.unselected().begin(); it != mit.unselected().end(); ++it)
				ctnr.insert(ctnr.end(), *it);
		return ctnr;
	}


	inline void select(UnselectedIterator iter) {
		if (iter != _unselected.end()) {
			ParamType k = *iter;
			_selected.insert(_selected.end(), std::make_pair(k, *iter));
			_unselected.erase(iter);
		}
	}


	inline void unselect(const ParamType& key) {
		unselect(_selected.find(key));
	}


	inline void unselect(const KeyIterator& iter) {
		if (iter != _selected.end()) {
			_unselected.insert(_unselected.end(), iter->second);
			_selected.erase(iter);
		}
	}


	inline UnselectedIterator find(SelectionType& item) {
		return std::find(_unselected.begin(), _unselected.end(), item);
	}


protected:
	virtual inline UnselectedIterator addNew(SelectionType& item) {
		UnselectedIterator ret = _unselected.insert(_unselected.end(), item);
		return ret;
	}


private:
	SelectionMap _selected;
	SelectionList _unselected;
};


}
}
}

#endif /* UKOCT_OPENCL_COMMON_POOL_HPP_ */
