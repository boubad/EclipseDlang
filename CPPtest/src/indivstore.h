#pragma once
#ifndef __INDIVSTORE_H__
#define __INDIVSTORE_H__
////////////////////////////////////
#include "indivprovider.h"
#include "distancemap.h"
#include "matdata.h"
/////////////////////////////////////
namespace info {
	/////////////////////////////////////
	template <typename T = int, typename U = int,
		class COMPAREU = std::less<U>,
		class ALLOCT = std::allocator<T>,
		class ALLOCU = std::allocator<U>,
		class ALLOCDOUBLE = std::allocator<double>,
		class ALLOCPTR = std::allocator<std::pair<U, std::shared_ptr<Indiv<T, U, ALLOCT>> > >
	> class IndivStore : public BaseInfo, public IIndivProvider<T, U, ALLOCT> {
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
		typedef IndivStore<T, U, COMPAREU, ALLOCT, ALLOCU, ALLOCDOUBLE, ALLOCPTR>  IndivStoreType;
	protected:
		size_t _currentindex;
		IndexTypeVector _keys;
		IndivTypePtrMap _map;
	public:
		  //
		
	
	public:
		IndivStore() :_currentindex(0) {}
		template <typename Z, typename W, class ALLOCZ, class ALLOCW, class ALLOCSTRING>
		IndivStore(const size_t nRows, const size_t nCols,
			const std::vector<Z, ALLOCZ> &data,
			const std::vector<StringType, ALLOCSTRING> *pNames = nullptr,
			const std::vector<W, ALLOCW> *pIndexes = nullptr) :
			_currentindex(0) {
			assert(nRows > 0);
			assert(nCols > 0);
			assert(data.size() >= (size_t)(nRows * nCols));
			IndexTypeVector &oVec = this->_keys;
			IndivTypePtrMap &oMap = this->_map;
			for (size_t irow = 0; irow < nRows; ++irow) {
				U aIndex = (U)(irow + 1);
				if ((pIndexes != nullptr) && (irow < pIndexes->size())) {
					aIndex = (U)(*pIndexes)[irow];
					while (aIndex < 1) {
						++aIndex;
					}// while
				} // pIndexes
				while (oMap.find(aIndex) != oMap.end()) {
					++aIndex;
				}// while
				StringType sId;
				if ((pNames != nullptr) && (irow < pNames->size())) {
					sId = (*pNames)[irow];
				}// pNames
				if (sId.empty()) {
					StringStreamType os;
					os << STRING_D_ID << aIndex;
					sId = os.str();
				}// empty
				DataTypeVector xdata(nCols);
				for (size_t j = 0; j < nCols; ++j) {
					xdata[j] = (T)data[irow * nCols + j];
				}// j
				IndivTypePtr oInd = std::make_shared<IndivType>(aIndex, sId, xdata);
				const IndivType *p = oInd.get();
				assert(p != nullptr);
				assert(p->is_valid());
				oMap[aIndex] = oInd;
				oVec.push_back(aIndex);
			}// irow

		}// IndivStore
		IndivStore(IIndivProvider *pProvider) :_currentindex(0) {
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
		}// IndivStore
		IndivStore(const IndivStoreType & other) :BaseInfo(other), _currentindex(other._currentindex),
			_keys(other._keys), _map(other._map) {}
		IndivStoreType & operator=(const IndivStoreType & other) {
			if (this != &other) {
				BaseInfo::operator=(other);
				this->_currentindex = other._currentindex;
				this->_keys = other._keys;
				this->_map = other._map;
			}
			return (*this);
		}
		virtual ~IndivStore() {}
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
		}// next
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
			}//
			return (nullptr);
		}// find
		const IndivType *find_at_index(const size_t i) const {
			assert(i < this->_keys.size());
			return (this->find(this->_keys[i]));
		}// find_at_index
		void keys(IndexTypeSet &oSet) const {
			oSet.clear();
			const IndexTypeVector &v = this->_keys;
			for (auto it = v.begin(); it != v.end(); ++it) {
				oSet.insert(*it);
			}// it
		}// keys
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
			}
			else {
				oMap[aIndex] = oo;
				this->_keys.push_back(aIndex);
			}
			return (true);
		}// add
		template <typename Z>
		void distance(const U index1, const U index2, Z &res,
			const DistanceFunc<Z> *pFunc = nullptr,
			const size_t irow1 = 0,
			const size_t irow2 = 0) const {
			const IndivType *p1 = this->get(index1);
			assert(p1 != nullptr);
			const IndivType *p2 = this->get(index2);
			assert(p2 != nullptr);
			p1->distance(*p2, res, pFunc, irow1, irow2);
		}//distance
		template <typename Z>
		void distance_index(const size_t i1, const size_t i2, Z &res,
			const DistanceFunc<Z> *pFunc = nullptr,
			const size_t irow1 = 0,
			const size_t irow2 = 0) const {
			assert(i1 < this->count());
			assert(i2 < this->count());
			this->distance(this->_keys[i1], this->_keys[i2], res, pFunc, irow1, irow2);
		}// distance_index
		template <typename Z, class ALLOCZ>
		void compute_distances(std::vector<Z, ALLOCZ> &oRes,
			const DistanceFunc<Z> *pFunc = nullptr) const {
			const size_t n = this->count();
			assert(n > 0);
			oRes.resize(n * n);
			for (size_t i = 0; i < n; ++i) {
				oRes[i * n + i] = (Z)0;
				for (size_t j = 0; j < i; ++j) {
					Z res = 0;
					this->distance_index(i, j, res, pFunc, i, j);
					oRes[i * n + j] = res;
					oRes[j * n + i] = res;
				}// j
			}// i
		}// compute_distance
		template <typename Z, class ALLOCZ>
		void compute_distances(std::vector<Z, ALLOCZ> &oRes,
			const DistanceType mode = DistanceType::distanceSquare) const {
			switch (mode) {
			case DistanceType::distanceDivergence:
			{
				DivergenceDistanceFunc<Z> fDist;
				this->compute_distances(oRes, &fDist);
			}
			break;
			case DistanceType::distanceEuclide:
			{
				EuclideDistanceFunc<Z> fDist;
				this->compute_distances(oRes, &fDist);
			}
			break;
			case DistanceType::distanceManhattan:
			{
				ManhattanDistanceFunc<Z> fDist;
				this->compute_distances(oRes, &fDist);
			}
			break;
			case DistanceType::distanceMax:
			{
				MaxDistanceFunc<Z> fDist;
				this->compute_distances(oRes, &fDist);
			}
			break;
			case DistanceType::distanceSquare:
			{
				DistanceFunc<Z> fDist;
				this->compute_distances(oRes, &fDist);
			}
			break;
			case DistanceType::distanceKhiDeux:
			{
				const size_t nRows = this->count();
				assert(nRows > 1);
				const IndivType *p0 = this->find_at_index(0);
				assert(p0 != nullptr);
				const size_t nCols = p0->size();
				assert(nCols > 1);
				const double zero = 0;
				std::vector<double, ALLOCDOUBLE> wcols(nCols, zero), wrows(nRows, zero);
				double total = 0;
				for (size_t irow = 0; irow < nRows; ++irow) {
					const IndivType *p = this->find_at_index(irow);
					assert(p != nullptr);
					const DataTypeVector &data = p->value();
					assert(data.size() >= nCols);
					for (size_t j = 0; j < nCols; ++j) {
						double x = (double)data[j];
						total += x;
						wcols[j] += x;
						wrows[i] += x;
					}// j
				}// irow
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
			case DistanceType::distanceVariance:
			{
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
						double x = (double)data[j];
						s1[j] += x;
						s2[j] += x * x;
					}// j
				}// irow
				for (size_t i = 0; i < nCols; ++i) {
					const double mx = s1[i] / nRows;
					double sx = (s2[i] / nRows) - (mx * mx);
					assert(sx > 0);
					s2[i] = 1.0 / sx;
				}// i
				VarianceDistanceFunc<Z> fDist(s2);
				this->compute_distances(oRes, &fDist);
			}
			break;
			default:
				// should not fall here
				assert(false);
				break;
			}// mode
		}// compute_distance
		template <typename Z, class ALLOCPM1, class ALLOCPM2>
		void compute_distances(DistanceMap<Z, ALLOCPM1, ALLOCPM2> &oDist,
			DistanceType mode = DistanceType::distanceSquare,
			Z zMax = 1000) const {
			assert(zMax > 0);
			std::vector<double, ALLOCDOUBLE> temp;
			this->compute_distances(temp, mode);
			const size_t n = this->count();
			assert(temp.size() >= (size_t)(n * n));
			const size_t nn = temp.size();
			double xmin = 0, xmax = 0;
			for (size_t i = 0; i < nn; ++i) {
				double x = temp[i];
				if (i == 0) {
					xmin = x;
					xmax = x;
				}
				else if (x < xmin) {
					xmin = x;
				}
				else if (x > xmax) {
					xmax = x;
				}
			}// i
			assert(xmin < xmax);
			double delta = (double)zMax / (xmax - xmin);
			const IndexTypeVector &kkeys = this->_keys;
			assert(kkeys.size() == n);
			for (size_t i = 0; i < n; ++i) {
				const U index1 = kkeys[i];
				for (size_t j = 0; j < i; ++j) {
					const U index2 = kkeys[j];
					double xx = temp[i * n + j];
					Z zx = (Z)((xx - xmin) * delta);
					oDist.add(j, i, zx);
				}// j
			}// i
		}// compute_distance
		virtual OStreamType & write_to(OStreamType &os) const {
			os << START_OBJECT;
			info_write_json_value(os, STRING_CURRENTINDEX, this->_currentindex) << STRING_COMMA;
			os << STRING_QUOTE << STRING_KEYS << STRING_QUOTE_END;
			info_write_json_array(os, this->_keys) << STRING_COMMA << std::endl;
			os << STRING_QUOTE << STRING_ALL_INDIVS << STRING_QUOTE_END << START_ARRAY;
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
			}// it
			os << END_ARRAY << std::endl;
			os << END_OBJECT;
			return (os);
		}// write_to
	};// class IndivStore<T, U, COMPAREU, ALLOCT, ALLOCU, ALLOCDOUBLE,ALLOCPTR>
	  /////////////////////////
	///////////////////////////////////////
}// namespace info
///////////////////////////////////////
#endif // !__INDIVSTORE_H__
