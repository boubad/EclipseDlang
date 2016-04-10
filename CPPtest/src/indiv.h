#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////
#include "infobase.h"
#include "distance.h"
/////////////////////////////////
#if defined(INFO_STRING_TYPE)
#define STRING_INDEX ("index")
#define STRING_ID ("id")
#define STRING_DATA ("data")
#define STRING_CURRENTINDEX ("currentindex")
#define STRING_KEYS ("keys")
#define STRING_ALL_INDIVS ("indivs")
#else
#define STRING_INDEX (L"index")
#define STRING_ID (L"id")
#define STRING_DATA (L"data")
#define STRING_CURRENTINDEX (L"currentindex")
#define STRING_KEYS (L"keys")
#define STRING_ALL_INDIVS (L"indivs")
#endif // INFO_STRING_TYPE
////////////////////////////////
namespace info {
enum class DistanceMode {
	modeInvalid, modeCenter, modeUpUp, modeUpDown, modeDownUp, modeDownDown
};
// enum DistanceMode
enum class LinkMode {
	noLink, linkMin, linkMax, linkMean, linkCenter
};
//////////////////////////////////////////
template<typename T = int, typename U = int, class ALLOCT = std::allocator<T> > class Indiv: BaseInfo {
	static_assert(std::is_integral<U>::value, "index typename must be integral type");
public:
	typedef T DataType;
	typedef U IndexType;
	//
	typedef std::vector<DataType, ALLOCT> DataTypeVector;
	typedef Indiv<DataType, IndexType, ALLOCT> IndivType;
private:
	IndexType _index;
	StringType _id;
	DataTypeVector _data;
public:
	Indiv() :
			_index(0) {
	}
	template<class CONTAINER>
	Indiv(const IndexType aIndex, const StringType &sid, const CONTAINER &v) :
			_index(aIndex), _id(sid), _data(v.size()) {
		const size_t n = v.size();
		assert(this->_data.size() == n);
		for (size_t i = 0; i < n; ++i) {
			this->_data[i] = (DataType) v[i];
		} // i
	}
	Indiv(const IndivType &other) :
			BaseInfo(other), _index(other._index), _id(other._id), _data(
					other._data) {
	}
	IndivType & operator=(const IndivType &other) {
		if (this != &other) {
			BaseInfo::operator=(other);
			this->_index = other._index;
			this->_id = other._id;
			this->_data = other._data;
		}
		return (*this);
	}
	virtual ~Indiv() {
	}
public:
	virtual int compare_to(const IndivType &other) const {
		if (this->index() < other.index()) {
			return (-1);
		} else if (this->index() > other.index()) {
			return (1);
		} else {
			return (0);
		}
	} // compare_to
	bool operator==(const IndivType &other) const {
		return (this->compare_to(other) == 0);
	}
	bool operator<(const IndivType &other) const {
		return (this->compare_to(other) < 0);
	}
public:
	virtual bool is_valid(void) const {
		return ((this->index() >= 0) && (this->size() > 0));
	}
	virtual const DataTypeVector & value(void) const {
		return (this->_data);
	}
	template<class CONTAINER>
	void value(const CONTAINER &v) {
		const size_t n = v.size();
		this->_data.resize(n);
		for (size_t i = 0; i < n; ++i) {
			this->_data[i] = (DataType) v[i];
		} // i
	}
	virtual IndexType index(void) const {
		return (this->_index);
	}
	virtual void index(const IndexType aIndex) {
		this->_index = aIndex;
	}
	virtual const StringType & id(void) const {
		return (this->_id);
	}
	virtual void id(const StringType &s) {
		this->_id = s;
	}
	//
	size_t size(void) const {
		return (this->value().size());
	}
	double norm(void) const {
		const DataTypeVector &t = this->value();
		double res = 0;
		const size_t n = t.size();
		for (size_t i = 0; i < n; ++i) {
			double tx = t[i];
			res += tx * tx;
		}
		if (n > 0) {
			res /= n;
		}
		return (std::sqrt(res));
	} // trace
	template<typename Z, typename TX, typename UX, class ALLOCX>
	void distance(const Indiv<TX, UX, ALLOCX> &other, Z &result,
			const DistanceFunc<Z> *pFunc = nullptr, const size_t irow1 = 0,
			const size_t irow2 = 0) const {
		info_distance(this->value(), other.value(), result, pFunc, irow1,
				irow2);
	} // distance
	template<typename Z, class CONTAINER>
	void distance(const CONTAINER &vv1, Z &result,
			const DistanceFunc<Z> *pFunc = nullptr, const size_t irow1 = 0,
			const size_t irow2 = 0) const {
		info_distance(this->value(), vv1, result, pFunc, irow1, irow2);
	} // distance
public:
	virtual OStreamType & write_to(OStreamType &os) const {
		os << START_OBJECT;
		info_write_json_value(os, STRING_INDEX, this->index()) << STRING_COMMA;
		info_write_json_value(os, STRING_ID, this->id()) << STRING_COMMA;
		info_write_json_value(os, STRING_DATA, this->value());
		os << END_OBJECT;
		return (os);
	} // write_to
};
// class Indiv<T,U,ALLOCT>
//////////////////////////////////
template<typename T = int, typename U = int, class ALLOCT = std::allocator<T> > class IIndivProvider {
	static_assert(std::is_integral<U>::value, "index typename must be integral type");
public:
	typedef T DataType;
	typedef U IndexType;
	//
	typedef Indiv<DataType, IndexType, ALLOCT> IndivType;
	typedef std::shared_ptr<IndivType> IndivTypePtr;
	typedef IIndivProvider<DataType, IndexType, ALLOCT> IIndivProviderType;
public:
	IIndivProvider() {
	}
	IIndivProvider(const IIndivProviderType & /*other*/) {
	}
	IIndivProviderType & operator=(const IIndivProviderType & /*other*/) {
		return (*this);
	}
	virtual ~IIndivProvider() {
	}
public:
	virtual void reset(void) = 0;
	virtual bool next(IndivTypePtr &oInd) = 0;
};
// class IndivProvider<T,U,ALLOCT>
///////////////////////////////////////
template<typename T = int, typename U = int, class COMPAREU = std::less<U>,
		class ALLOCT = std::allocator<T>, class ALLOCU = std::allocator<U>,
		class ALLOCDOUBLE = std::allocator<double>,
		class ALLOCPTR = std::allocator<
				std::pair<U, std::shared_ptr<Indiv<T, U, ALLOCT>> > > > class IndivStore: public BaseInfo,
		public IIndivProvider<T, U, ALLOCT> {
	static_assert(std::is_integral<U>::value, "index typename must be integral type");
public:
	typedef T DataType;
	typedef U IndexType;
	//
	typedef std::vector<T, ALLOCT> DataTypeVector;
	typedef Indiv<DataType, IndexType, ALLOCT> IndivType;
	typedef std::deque<U, ALLOCU> IndexTypeVector;
	typedef std::set<U, COMPAREU, ALLOCU> IndexTypeSet;
	typedef std::shared_ptr<IndivType> IndivTypePtr;
	typedef std::map<U, IndivTypePtr, COMPAREU, ALLOCPTR> IndivTypePtrMap;
	typedef IIndivProvider<DataType, IndexType, ALLOCT> IIndivProviderType;
	typedef IndivStore<T, U, COMPAREU, ALLOCT, ALLOCU, ALLOCDOUBLE, ALLOCPTR> IndivStoreType;
public:
	template<typename Z, class ALLOCPM1, class ALLOCPM2>
	class DistanceMap {
	private:
		std::map<U, std::map<U, Z, COMPAREU, ALLOCPM1>, COMPAREU, ALLOCPM2> oMap;
	public:
		DistanceMap() {
		}
		DistanceMap(
				const std::map<U, std::map<U, Z, COMPAREU, ALLOCPM1>, COMPAREU,
						ALLOCPM2> &m) :
				oMap(m) {
		}
		~DistanceMap() {
		}
	public:
		void add(const U index1, const U index2, const Z res) {
			U i1 = index1, i2 = index2;
			if (i1 > i2) {
				const U t = i1;
				i1 = i2;
				i2 = t;
			}
			auto it1 = oMap.find(i1);
			if (it1 == oMap.end()) {
				const std::map<U, Z, COMPAREU, ALLOCPM1> m;
				m[i2] = res;
				oMap[i1] = m;
			} else {
				const std::map<U, Z, COMPAREU, ALLOCPM1> &m = (*it1).second;
				m[i2] = res;
			}
		}	// add
		bool find(const U index1, const U index2, Z &res) const {
			U i1 = index1, i2 = index2;
			if (i1 > i2) {
				const U t = i1;
				i1 = i2;
				i2 = t;
			}
			auto it1 = oMap.find(i1);
			if (it1 == oMap.end()) {
				return (false);
			}
			const std::map<U, Z, COMPAREU, ALLOCPM1> &m = (*it1).second;
			auto jt = m.find(i2);
			if (jt != m.end()) {
				res = (*jt).second();
				return (true);
			}
			return (false);
		}	// find
	};	// distanceMap;
		//
	class IndexedCluster: public IndivType {
	public:
		IndexedCluster() :
				_pstore(nullptr) {
		}
		IndexedCluster(const IndivStoreType *pStore, const IndexType aIndex,
				const StringType &sid, const DataTypeVector &v) :
				IndivType(aIndex, sid, v), _pstore(pStore) {
			this->_members.push_back(aIndex);
		}
		IndexedCluster(const IndivStoreType *pStore, const IndivType &oIndiv) :
				IndivType(oIndiv), _pstore(pStore) {
			this->_members.push_back(oIndiv.index());
		}
		IndexedCluster(const IndexedCluster &other) :
				IndivType(other), _pstore(other._pstore), _members(
						other._members) {
		}
		IndexedCluster & operator=(const IndexedCluster &other) {
			if (this != &other) {
				IndivType::operator=(other);
				this->_pstore = other._pstore;
				this->_members = other._members;
			}
			return (*this);
		}
		virtual ~IndexedCluster() {
		}
	public:
		virtual bool is_valid(void) const {
			return (IndivType::is_valid() && (this->_pstore != nullptr));
		}
		bool empty(void) const {
			return (this->_members.empty());
		}
		void clear_members(void) {
			this->update_center();
			this->_members.clear();
		}		// clear_members
		void merge(const IndexedCluster &other, DistanceMode mode =
				DistanceMode::modeUpUp) {
			if (other.empty()) {
				return;
			}
			IndexTypeVector & vdest = this->_members;
			const IndexTypeVector & vsrc = other._members;
			const size_t n = vsrc.size();
			switch (mode) {
			case DistanceMode::modeUpUp:
				for (size_t i = 0; i < n; ++i) {
					vdest.push_front(vsrc[i]);
				}		// i
				break;
			case DistanceMode::modeUpDown:
				for (size_t i = (size_t) (n - 1); i >= 0; --i) {
					vdest.push_front(vsrc[i]);
					if (i < 1) {
						break;
					}
				}		// i
				break;
			case DistanceMode::modeDownUp:
				for (size_t i = 0; i < n; ++i) {
					vdest.push_back(vsrc[i]);
				}		// i
				break;
			case DistanceMode::modeDownDown:
				for (size_t i = (size_t) (n - 1); i >= 0; --i) {
					vdest.push_back(vsrc[i]);
					if (i < 1) {
						break;
					}
				}		// i
				break;
			default:
				// should not be here
				assert(false);
				break;
			}		// mode
		}		// merge
		template<typename Z, class ALLOCPM1, class ALLOCPM2>
		bool arrange_distance(const IndexedCluster &other,
				const DistanceMap<Z, ALLOCPM1, ALLOCPM2> &oDist,
				std::pair<Z, DistanceMode> &res) const {
			const IndexTypeVector & vv1 = this->_members;
			const IndexTypeVector & vv2 = other._members;
			const size_t n1 = vv1.size();
			const size_t n2 = vv2.size();
			res.second = DistanceMode::modeUpUp;
			res.first = 0;
			if ((n1 < 1) || (n2 < 1)) {
				return (false);
			}
			if ((n1 == 1) && (n2 == 1)) {
				oDist.find(vv1[0], vv2[0], res.first);
			} else if ((n1 == 1) && (n2 > 1)) {
				oDist.find(vv1[0], vv2[0], res.first);
				Z dd = 0;
				oDist.find(vv1[0], vv2[n2 - 1], dd);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeUpDown;
				}
			} else if ((n1 > 1) && (n2 == 1)) {
				oDist.find(vv1[0], vv2[0], res.first);
				Z dd = 0;
				oDist.find(vv1[n1 - 1], vv2[0], dd);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeDownUp;
				}
			} else {
				oDist.find(vv1[0], vv2[0], res.first);
				Z dd = 0;
				oDist.find(vv1[0], vv2[n2 - 1], dd);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeUpDown;
				}
				oDist.find(vv1[n1 - 1], vv2[0], dd);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeDownUp;
				}
				oDist.find(vv1[n1 - 1], vv2[n2 - 1], dd);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeDownDown;
				}
			}
			return (true);
		}		// arrange_distance
		template<typename Z>
		bool arrange_distance(const IndexedCluster &other,
				std::pair<Z, DistanceMode> &res, const DistanceFunc<Z> *pFunc =
						nullptr) const {
			const IndivStoreType *pStore = this->_pstore;
			assert(pStore != nullptr);
			const IndexTypeVector & vv1 = this->_members;
			const IndexTypeVector & vv2 = other._members;
			const size_t n1 = vv1.size();
			const size_t n2 = vv2.size();
			res.second = DistanceMode::modeUpUp;
			res.first = 0;
			if ((n1 < 1) || (n2 < 1)) {
				return (false);
			}
			if ((n1 == 1) && (n2 == 1)) {
				pStore->distance(vv1[0], vv2[0], res.first, pFunc);
			} else if ((n1 == 1) && (n2 > 1)) {
				pStore->distance(vv1[0], vv2[0], res.first, pFunc);
				Z dd = 0;
				pStore->distance(vv1[0], vv2[n2 - 1], dd, pFunc);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeUpDown;
				}
			} else if ((n1 > 1) && (n2 == 1)) {
				pStore->distance(vv1[0], vv2[0], res.first, pFunc);
				Z dd = 0;
				pStore->distance(vv1[n1 - 1], vv2[0], dd, pFunc);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeDownUp;
				}
			} else {
				pStore->distance(vv1[0], vv2[0], res.first, pFunc);
				Z dd = 0;
				pStore->distance(vv1[0], vv2[n2 - 1], dd, pFunc);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeUpDown;
				}
				pStore->distance(vv1[n1 - 1], vv2[0], dd, pFunc);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeDownUp;
				}
				pStore->distance(vv1[n1 - 1], vv2[n2 - 1], dd, pFunc);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeDownDown;
				}
			}
			return (true);
		}		//arrange_distance

		template<typename Z>
		void distance(const IndexedCluster &other, Z &res, LinkMode link =
				LinkMode::linkCenter, const DistanceFunc<Z> *pFunc = nullptr,
				const size_t irow1 = 0, const size_t irow2 = 0) const {
			assert(link != LinkMode::noLink);
			res = 0;
			if (this->empty() || other.empty()) {
				return;
			}
			if (link == LinkMode::linkCenter) {
				info_distance(this->value(), other.value(), res, pFunc, irow1,
						irow2);
				return;
			}
			if ((pFunc != nullptr)
					&& (pFunc->type() == DistanceType::distanceKhiDeux)) {
				assert(false);
				return;
			}
			const IndivStoreType *pStore = this->_pstore;
			assert(pStore != nullptr);
			const IndexTypeVector & vv1 = this->_members;
			const IndexTypeVector & vv2 = other._members;
			const size_t n1 = vv1.size();
			const size_t n2 = vv2.size();
			if ((n1 == 1) && (n2 == 1)) {
				pStore->distance(vv1[0], vv2[0], res, pFunc);
				return;
			}
			if ((n2 == 1) && (n1 > 1)) {
				other.distance(*this, res, link, pFunc);
				return;
			}
			if ((n1 == 1) && (n2 > 1)) {
				double total = 0.0;
				const IndivType *pFirst = pStore->find(vv1[0]);
				assert(pFirst != nullptr);
				for (size_t i = 0; i < n2; ++i) {
					const IndivType *p = pStore->find(vv2[i]);
					assert(p != nullptr);
					Z dx = 0;
					pFirst->distance(*p, dx, pFunc);
					switch (link) {
					case LinkMode::linkMin:
						if (i == 0) {
							res = dx;
						} else if (dx < res) {
							res = dx;
						}
						break;
					case LinkMode::linkMax:
						if (i == 0) {
							res = dx;
						} else if (dx > res) {
							res = dx;
						}
						break;
					case LinkMode::linkMean:
						total += (double) dx;
						break;
					default:
						// should not been here
						assert(false);
						break;
					}		// link
				}		// i
				if (link == LinkMode::linkMean) {
					res = (Z) (total / n2);
				}
				return;
			}
			if ((n1 > 1) && (n2 > 1)) {
				double total = 0.0;
				size_t nCount = 0;
				for (size_t i = 0; i < n1; ++i) {
					const IndivType *p1 = pStore->find(vv1[i]);
					assert(p1 != nullptr);
					for (size_t j = 0; j < n2; ++j) {
						const IndivType *p2 = pStore->find(vv2[j]);
						assert(p2 != nullptr);
						Z dx = 0;
						p1->distance(*p2, dx, pFunc);
						++nCount;
						switch (link) {
						case LinkMode::linkMin:
							if ((i == 0) && (j == 0)) {
								res = dx;
							} else if (dx < res) {
								res = dx;
							}
							break;
						case LinkMode::linkMax:
							if ((i == 0) && (j == 0)) {
								res = dx;
							} else if (dx > res) {
								res = dx;
							}
							break;
						case LinkMode::linkMean:
							total += (double) dx;
							break;
						default:
							// should not been here
							assert(false);
							break;
						}		// link
					}		// i
					if (link == LinkMode::linkMean) {
						res = (Z) (total / nCount);
					}
				}
			}
		}		// distance
		void add(const IndivType &oIndiv, bool bUpdate = false) {
			assert(this->is_valid());
			IndexTypeVector & vv = this->_members;
			const U aIndex = oIndiv.index();
			const size_t n = vv.size();
			if (n < 2) {
				vv.push_back(aIndex);
				if (bUpdate) {
					this->update_center();
				}
				return;
			}
			const IndivStoreType *pStore = this->_pstore;
			assert(pStore != nullptr);
			double d1 = 0, d2 = 0;
			pStore->distance(aIndex, vv[0], d1);
			pStore->distance(aIndex, vv[n - 1], d2);
			if (d1 <= d2) {
				vv.pop_front(aIndex);
			} else {
				vv.push_back(aIndex);
			}
			if (bUpdate) {
				this->update_center();
			}
		}		// add
		void update_center(void) {
			assert(this->is_valid());
			IndexTypeVector & vv = this->_members;
			if (vv.empty()) {
				return;
			}
			const size_t n = vv.size();
			assert(n > 0);
			const IndivStoreType *pStore = this->_pstore;
			assert(pStore != nullptr);
			const U aFirst = vv[0];
			const IndivType *pFirst = pStore->find(aFirst);
			assert(pFirst != nullptr);
			const size_t nCols = pFirst->size();
			assert(nCols > 0);
			const double zero = 0.0;
			std::vector<double, ALLOCDOUBLE> som(nCols, zero);
			for (size_t i = 0; i < n; ++i) {
				const U aIndex = vv[i];
				const IndivType *p = pStore->find(aIndex);
				assert(p != nullptr);
				const DataTypeVector &d = p->value();
				assert(d.size() >= nCols);
				for (size_t j = 0; j < nCols; ++j) {
					som[j] += (double) d[j];
				}		// j
			}		// i
			for (size_t i = 0; i < nCols; ++i) {
				som[i] /= n;
			}
			this->value(som);
		}		// update_center
	protected:
		const IndivStoreType *_pstore;
		IndexTypeVector _members;
	};		// class IndexedCluster
protected:
	size_t _currentindex;
	IndexTypeVector _keys;
	IndivTypePtrMap _map;
public:
	IndivStore() :
			_currentindex(0) {
	}
	template<typename Z, typename W, class ALLOCZ, class ALLOCW,
			class ALLOCSTRING>
	IndivStore(const size_t nRows, const size_t nCols,
			const std::vector<Z, ALLOCZ> &data,
			const std::vector<StringType, ALLOCSTRING> *pNames = nullptr,
			const std::vector<W, ALLOCW> *pIndexes = nullptr) :
			_currentindex(0) {
		assert(nRows > 0);
		assert(nCols > 0);
		assert(data.size() >= (size_t )(nRows * nCols));
		IndexTypeVector &oVec = this->_keys;
		IndivTypePtrMap &oMap = this->_map;
		for (size_t irow = 0; irow < nRows; ++irow) {
			U aIndex = (U) (irow + 1);
			if ((pIndexes != nullptr) && (irow < pIndexes->size())) {
				aIndex = (U) (*pIndexes)[irow];
				while (aIndex < 1) {
					++aIndex;
				}		// while
			} // pIndexes
			while (oMap.find(aIndex) != oMap.end()) {
				++aIndex;
			} // while
			StringType sId;
			if ((pNames != nullptr) && (irow < pNames->size())) {
				sId = (*pNames)[irow];
			} // pNames
			if (sId.empty()) {
				StringStreamType os;
				os << STRING_D_ID << aIndex;
				sId = os.str();
			} // empty
			DataTypeVector xdata(nCols);
			for (size_t j = 0; j < nCols; ++j) {
				xdata[j] = (T) data[irow * nCols + j];
			} // j
			IndivTypePtr oInd = std::make_shared<IndivType>(aIndex, sId, xdata);
			const IndivType *p = oInd.get();
			assert(p != nullptr);
			assert(p->is_valid());
			oMap[aIndex] = oInd;
			oVec.push_back(aIndex);
		} // irow
	} // IndivStore
	IndivStore(IIndivProviderType *pProvider) :
			_currentindex(0) {
		assert(pProvider != nullptr);
		pProvider->reset();
		IndexTypeVector &oVec = this->_keys;
		IndivTypePtrMap &oMap = this->_map;
		do {
			IndivTypePtr oInd;
			if (!pProvider->next(oInd)) {
				break;
			}
			const IndivType *p = oInd.get();
			assert(p != nullptr);
			assert(p->is_valid());
			const IndexType aIndex = p->index();
			if (oMap.find(aIndex) == oMap.end()) {
				oMap[aIndex] = oInd;
				oVec.push_back(aIndex);
			}
		} while (true);
	} // IndivStore
	IndivStore(const IndivStoreType & other) :
			BaseInfo(other), _currentindex(other._currentindex), _keys(
					other._keys), _map(other._map) {
	}
	IndivStoreType & operator=(const IndivStoreType & other) {
		if (this != &other) {
			BaseInfo::operator=(other);
			this->_currentindex = other._currentindex;
			this->_keys = other._keys;
			this->_map = other._map;
		}
		return (*this);
	}
	virtual ~IndivStore() {
	}
public:
	virtual void reset(void) {
		this->_currentindex = 0;
	}
	virtual bool next(IndivTypePtr &oInd) {
		if (this->_currentindex >= this->_keys.size()) {
			oInd.reset();
			return (false);
		}
		const U aIndex = this->_keys[this->_currentindex++];
		if (this->_map.find(aIndex) != this->_map.end()) {
			oInd = this->_map[aIndex];
			return (true);
		}
		oInd.reset();
		return (false);
	} // next
	size_t count(void) const {
		return (this->_keys.size());
	}
	const IndivType *find(const IndexType aIndex) const {
		auto it = this->_map.find(aIndex);
		if (it != this->_map.end()) {
			IndivTypePtr o = (*it).second;
			const IndivType *pInd = o.get();
			assert(pInd != nullptr);
			return (pInd);
		} //
		return (nullptr);
	} // find
	const IndivType *find_at_index(const size_t i) const {
		assert(i < this->_keys.size());
		return (this->find(this->_keys[i]));
	} // find_at_index
	void keys(IndexTypeSet &oSet) const {
		oSet.clear();
		const IndexTypeVector &v = this->_keys;
		for (auto it = v.begin(); it != v.end(); ++it) {
			oSet.insert(*it);
		} // it
	} // keys
	bool add(const IndivTypePtr &oInd) {
		const IndivType *p = oInd.get();
		if (p == nullptr) {
			return (false);
		}
		if (!p->is_valid()) {
			return (false);
		}
		IndivTypePtr oo(oInd);
		const IndexType aIndex = p->index();
		IndivTypePtrMap &oMap = this->_map;
		if (oMap.find(aIndex) != oMap.end()) {
			oMap[aIndex] = oo;
		} else {
			oMap[aIndex] = oo;
			this->_keys.push_back(aIndex);
		}
		return (true);
	} // add
	template<typename Z>
	void distance(const U index1, const U index2, Z &res,
			const DistanceFunc<Z> *pFunc = nullptr, const size_t irow1 = 0,
			const size_t irow2 = 0) const {
		const IndivType *p1 = this->get(index1);
		assert(p1 != nullptr);
		const IndivType *p2 = this->get(index2);
		assert(p2 != nullptr);
		p1->distance(*p2, res, pFunc, irow1, irow2);
	} //distance
	template<typename Z>
	void distance_index(const size_t i1, const size_t i2, Z &res,
			const DistanceFunc<Z> *pFunc = nullptr, const size_t irow1 = 0,
			const size_t irow2 = 0) const {
		assert(i1 < this->count());
		assert(i2 < this->count());
		this->distance(this->_keys[i1], this->_keys[i2], res, pFunc, irow1,
				irow2);
	} // distance_index
	template<typename Z, class ALLOCZ>
	void compute_distances(std::vector<Z, ALLOCZ> &oRes,
			const DistanceFunc<Z> *pFunc = nullptr) const {
		const size_t n = this->count();
		assert(n > 0);
		oRes.resize(n * n);
		for (size_t i = 0; i < n; ++i) {
			oRes[i * n + i] = (Z) 0;
			for (size_t j = 0; j < i; ++j) {
				Z res = 0;
				this->distance_index(i, j, res, pFunc, i, j);
				oRes[i * n + j] = res;
				oRes[j * n + i] = res;
			} // j
		} // i
	} // compute_distance
	template<typename Z, class ALLOCZ>
	void compute_distances(std::vector<Z, ALLOCZ> &oRes,
			const DistanceType mode = DistanceType::distanceSquare) const {
		switch (mode) {
		case DistanceType::distanceDivergence: {
			DivergenceDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceEuclide: {
			EuclideDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceManhattan: {
			ManhattanDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceMax: {
			MaxDistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceSquare: {
			DistanceFunc<Z> fDist;
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceKhiDeux: {
			const size_t nRows = this->count();
			assert(nRows > 1);
			const IndivType *p0 = this->find_at_index(0);
			assert(p0 != nullptr);
			const size_t nCols = p0->size();
			assert(nCols > 1);
			const double zero = 0;
			std::vector<double, ALLOCDOUBLE> wcols(nCols, zero), wrows(nRows,
					zero);
			double total = 0;
			for (size_t irow = 0; irow < nRows; ++irow) {
				const IndivType *p = this->find_at_index(irow);
				assert(p != nullptr);
				const DataTypeVector &data = p->value();
				assert(data.size() >= nCols);
				for (size_t j = 0; j < nCols; ++j) {
					double x = (double) data[j];
					total += x;
					wcols[j] += x;
					wrows[irow] += x;
				} // j
			} // irow
			assert(total > 0);
			for (size_t i = 0; i < nCols; ++i) {
				wcols[i] /= total;
			}
			for (size_t i = 0; i < nRows; ++i) {
				wrows[i] /= total;
			}
			Khi2DistanceFunc<Z> fDist(wrows, wcols);
			this->compute_distances(oRes, &fDist);
		}
			break;
		case DistanceType::distanceVariance: {
			const size_t nRows = this->count();
			assert(nRows > 1);
			const IndivType *p0 = this->find_at_index(0);
			assert(p0 != nullptr);
			const size_t nCols = p0->size();
			assert(nCols > 1);
			const double zero = 0;
			std::vector<double, ALLOCDOUBLE> s1(nCols, zero), s2(nCols, zero);
			for (size_t irow = 0; irow < nRows; ++irow) {
				const IndivType *p = this->find_at_index(irow);
				assert(p != nullptr);
				const DataTypeVector &data = p->value();
				assert(data.size() >= nCols);
				for (size_t j = 0; j < nCols; ++j) {
					double x = (double) data[j];
					s1[j] += x;
					s2[j] += x * x;
				} // j
			} // irow
			for (size_t i = 0; i < nCols; ++i) {
				const double mx = s1[i] / nRows;
				double sx = (s2[i] / nRows) - (mx * mx);
				assert(sx > 0);
				s2[i] = 1.0 / sx;
			} // i
			VarianceDistanceFunc<Z> fDist(s2);
			this->compute_distances(oRes, &fDist);
		}
			break;
		default:
			// should not fall here
			assert(false);
			break;
		} // mode
	} // compute_distance
	template<typename Z, class ALLOCPM1, class ALLOCPM2>
	void compute_distances(DistanceMap<Z, ALLOCPM1, ALLOCPM2> &oDist,
			DistanceType mode = DistanceType::distanceSquare,
			Z zMax = 1000) const {
		assert(zMax > 0);
		std::vector<double, ALLOCDOUBLE> temp;
		this->compute_distances(temp, mode);
		const size_t n = this->count();
		assert(temp.size() >= (size_t )(n * n));
		const size_t nn = temp.size();
		double xmin = 0, xmax = 0;
		for (size_t i = 0; i < nn; ++i) {
			double x = temp[i];
			if (i == 0) {
				xmin = x;
				xmax = x;
			} else if (x < xmin) {
				xmin = x;
			} else if (x > xmax) {
				xmax = x;
			}
		} // i
		assert(xmin < xmax);
		double delta = (double) zMax / (xmax - xmin);
		const IndexTypeVector &kkeys = this->_keys;
		assert(kkeys.size() == n);
		for (size_t i = 0; i < n; ++i) {
			const U index1 = kkeys[i];
			for (size_t j = 0; j < i; ++j) {
				const U index2 = kkeys[j];
				double xx = temp[i * n + j];
				Z zx = (Z) ((xx - xmin) * delta);
				oDist.add(j, i, zx);
			} // j
		} // i
	} // compute_distance
	virtual OStreamType & write_to(OStreamType &os) const {
		os << START_OBJECT;
		info_write_json_value(os, STRING_CURRENTINDEX,
				this->_currentindex) << STRING_COMMA;
		os << STRING_QUOTE << STRING_KEYS << STRING_QUOTE_END;
		info_write_json_array(os, this->_keys) << STRING_COMMA << std::endl;
		os << STRING_QUOTE << STRING_ALL_INDIVS << STRING_QUOTE_END
				<< START_ARRAY;
		const IndivTypePtrMap &oMap = this->_map;
		auto iStart = oMap.begin();
		for (auto it = iStart; it != oMap.end(); ++it) {
			os << std::endl;
			if (it != iStart) {
				os << STRING_COMMA;
			}
			IndivTypePtr oInd = (*it).second;
			const IndivType *p2 = oInd.get();
			assert(p2 != nullptr);
			os << *p2;
		} // it
		os << END_ARRAY << std::endl;
		os << END_OBJECT;
		return (os);
	} // write_to
};
// class IndivStore<T, U, COMPAREU, ALLOCT, ALLOCU, ALLOCDOUBLE,ALLOCPTR>
/////////////////////////
}// namespace info
///////////////////////////////////
template<typename T, typename U, class ALLOCT>
info::OStreamType & operator<<(info::OStreamType &os,
		const info::Indiv<T, U, ALLOCT> &d) {
	return (d.write_to(os));
}
template<typename T, typename U, class COMPAREU, class ALLOCT, class ALLOCU,
		class ALLOCDOUBLE, class ALLOCPTR>
info::OStreamType & operator<<(info::OStreamType &os,
		const info::IndivStore<T, U, COMPAREU, ALLOCT, ALLOCU, ALLOCDOUBLE,
				ALLOCPTR> &d) {
	return (d.write_to(os));
}
///////////////////////////////////
#endif // !__INDIV_H__
///////////////////////
//eof: indiv.h
