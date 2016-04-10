#pragma once
#ifndef __ELEMCRIT_H__
#define __ELEMCRIT_H__
///////////////////////////
#include "utils.h"
#include "indiv.h"
///////////////////////////////////
namespace info {
	//////////////////////////////////////////////////
	template <typename Z = long, typename U = size_t,
		class ALLOCU = std::allocator<U>,
		class Compare = std::less<U> >
		class ElemCrit {
		static_assert(std::is_integral<U>::value, "index typename must be integral type");
		public:
			typedef U IndexType;
			typedef Z DistanceType;
			//
			typedef std::set<U, Compare, ALLOCU> IndexTypeSet;
			//
			typedef ElemCrit<DistanceType, IndexType, ALLOCU, Compare> ElemCritType;
		private:
			DistanceMode _order;
			DistanceType _dist;
			IndexTypeSet _oSet;
		public:
			ElemCrit() : _order(DistanceMode::modeCenter), _dist(0) {
			}
			ElemCrit(const IndexType i1,
				const IndexType i2,
				const DistanceMode o = DistanceMode::modeCenter,
				const DistanceType d = 0) : _order(o), _dist(d) {
				IndexTypeSet &oSet = this->_oSet;
				oSet.insert(i1);
				oSet.insert(i2);
			}
			ElemCrit(const ElemCritType &other) :
				_order(other._order), _dist(other._dist), _oSet(other._oSet) {}
			ElemCritType & operator=(const ElemCritType &other) {
				if (this != &other) {
					this->_order = other._order;
					this->_dist = other._dist;
					this->_oSet = other._oSet;
				}
				return (*this);
			}
			~ElemCrit() {}
		public:
			bool empty(void) const {
				return (this->_oSet.empty());
			}
			const IndexTypeSet & indexSet(void) const {
				return (this->_oSet);
			}
			bool is_valid(void) const {
				return ((this->_order != DistanceMode::modeInvalid) && (this->_oSet.size() > 1));
			}
			bool contains(const IndexType aIndex) const {
				return (this->_oSet.find(aIndex) != this->_oSet.end());
			}
			IndexType first(void) const {
				const IndexTypeSet &oSet = this->_oSet;
				assert(!oSet.empty());
				return (*(oSet.begin()));
			}
			DistanceMode order(void) const {
				return (this->_order);
			}
			void order(const DistanceMode o) {
				this->_order = o;
			}
			DistanceType criteria(void) const {
				return (this->_dist);
			}
			void criteria(const DistanceType d) {
				this->_dist = d;
			}
			bool add(const IndexType i1, const IndexType i2) {
				IndexType i = i1, j = i2;
				bool bRet = false;
				IndexTypeSet &oSet = this->_oSet;
				if (i != j) {
					if (oSet.empty()) {
						oSet.insert(i);
						oSet.insert(j);
						return (true);
					}
					if (i > j) {
						const IndexType t = i;
						i = j;
						j = t;
					}// swap
					if (oSet.find(i) != oSet.begin()) {
						oSet.insert(j);
						bRet = true;
					}
				}// j != i
				return (bRet);
			}// add
		public:
			bool operator==(const ElemCritType &other) const {
				const IndexTypeSet & oSet1 = this->_oSet;
				const IndexTypeSet &oSet2 = other._oSet;
				auto iEnd1 = oSet1.end();
				auto iEnd2 = oSet2.end();
				for (auto it = oSet1.begin(); it != iEnd; ++it) {
					const IndexType v = *it;
					if (oSet2.find(v) != iEnd2) {
						return (true);
					}
				}
				return (false);
			}
	};// class ElemCrit<Z,U>
	  ///////////////////////////////////////////
	template <typename Z = long, typename U = size_t,
		class ALLOCU = std::allocator<U>,
		class Compare = std::less<U>,
		class ALLOCCRIT = std::allocator<std::pair<U, ElemCrit<Z, U, ALLOCU, Compare> > > > class ElemCrits {
		static_assert(std::is_integral<U>::value, "index typename must be integral type");
		public:
			typedef U IndexType;
			typedef Z DistanceType;
			//
			typedef std::set<U, Compare, ALLOCU> IndexTypeSet;
			typedef ElemCrit<DistanceType, IndexType, ALLOCU, Compare> ElemCritType;
			typedef std::map<IndexType, ElemCritType, Compare, ALLOCCRIT> ElemCritTypeMap;
			typedef ElemCrits<DistanceType, IndexType, ALLOCU, Compare, ALLOCCRIT> ElemCritsType;
		private:
			DistanceType _crit;
			ElemCritTypeMap  _data;
		public:
			ElemCrits() :_crit(0) {}
			ElemCrits(const ElemCritsType &other) :_crit(other._crit), _data(other._data) {}
			ElemCritsType & operator=(const ElemCritsType &other) {
				if (this != &other) {
					this->_crit = other._crit;
					this->_data = other._data;
				}
				return (*this);
			}
			~ElemCrits() {}
		public:
			void get_second_indexes(IndexTypeSet &oSet) const {
				oSet.clear();
				const ElemCritTypeVector &vv = this->_data;
				auto vEnd = vv.end();
				for (auto it = vv.begin(); it != vEnd; ++it) {
					const ElemCritType & v = (*it).second;
					if (!v.empty()) {
						const IndexTypeSet &xSet = v.indexSet();
						auto jEnd = v.end();
						auto jt = xSet.begin();
						if (jt != jEnd) {
							++jt;
							while (jt != jEnd) {
								const IndexType ic = *jt;
								oSet.insert(ic);
								++jt;
							}// jt
						}// j
					}// not empty
				}// it
			}//get_second_indexes
			bool operator<(const ElemCritsType &other) const {
				return (this->criteria() < other.criteria());
			}
			bool operator==(const ElemCritsType &other) const {
				return (this->criteria() == other.criteria());
			}
		public:
			const ElemCritTypeMap & elems(void) const {
				return (this->_data);
			}
			bool empty(void) const {
				return (this->_data.empty());
			}
			void clear(void) {
				this->_crit = 0;
				this->_data.clear();
			}// clear
			DistanceType criteria(void) const {
				return (this->_crit);
			}// criteria
			ElemCritsType & operator+=(const ElemCritType &c) {
				this->add(c);
				return (*this);
			}
			bool add(const ElemCritType &c) {
				if (!c.is_valid()) {
					return (false);
				}
				const IndexTypeSet &oSet = c.indexSet();
				auto it = oSet.begin();
				assert(it != oSet.end());
				const IndexType i = *it;
				++it;
				assert(it != oSet.end());
				const IndexType j = *it;
				return (this->add(i, j, c.order(), c.criteria()));
			}// add
			bool add(const IndexType i, const IndexType j, const DistanceMode m, const DistanceType crit) {
				IndexType i1 = i, i2 = j;
				if (i1 > i2) {
					const IndexType x = i1;
					i1 = i2;
					i2 = x;
				}
				if (i1 == i2) {
					return (false);
				}
				ElemCritTypeMap &vv = this->_data;
				if (vv.empty()) {
					this->_crit = crit;
					ElemCritType cc(i1, i2, m, crit);
					vv[i1] = cc;
					return (true);
				}
				const DistanceType bestCrit = this->_crit;
				if (crit > bestCrit) {
					return (false);
				}
				else if (crit < bestCrit) {
					vv.clear();
					this->_crit = crit;
					ElemCritType cc(i1, i2, m, crit);
					vv[i1] = cc;
					return (true);
				}
				if (vv.find(i1) != vv.end()) {
					ElemCritType &cc = vv[i1];
					return (cc.add(i1, i2));
				}
				auto iend = vv.end();
				for (auto it = vv.begin(); it != iend; ++it) {
					const ElemCritType &cc = (*it).second;
					if (cc.contains(i2)) {
						return (false);
					}
				}// it
				ElemCritType cx(i1, i2, m, crit);
				vv[i1] = cx;
				return (true);
			}// add
	};// class ElemCrits<Z,U,ALLOC>
	  ////////////////////////////////////////
}// namespace info
 ///////////////////////////////
#endif // !__ELEMCRIT_H__
 /////////////////////
 //eof: elemcrit.h
