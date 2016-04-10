#pragma once
#ifndef __CRITS_H__
#define  __CRITS_H__
////////////////////////////////////////
#include "utils.h"
#include "indiv.h"
//////////////////////////////////////////
namespace info {
	///////////////////////////////////
	template <typename U = int> class ElemCritEntry {
		static_assert(std::is_integral<U>::value, "index typename must be integral type");
	public:
		U	first;
		DistanceMode second;
		ElemCritEntry(const U aIndex = 0, const DistanceMode m = DistanceMode::modeCenter) :
			first(aIndex), second(m) {}
		ElemCritEntry(const ElemCritEntry<U> &other) :first(other.first), second(other.second) {}
		ElemCritEntry<U> & operator=(const ElemCritEntry<U> &other) {
			if (this != &other) {
				this->first = other.first;
				this->second = other.second;
			}
			return (*this);
		}
		virtual ~ElemCritEntry() {}
		bool operator<(const ElemCritEntry<U> &other) const {
			return (this->first < other.first);
		}
		bool operator==(const ElemCritEntry<U> &other) const {
			return (this->first == other.first);
		}
	}; // class ElemCritEntry
	   //////////////////////////////////////
	template <typename U = int,
		class CompareEntry = std::less<ElemCritEntry<U> >,
		class ALLOCC = std::allocator<ElemCritEntry<U> >
	> class ElemCritDesc {
	public:
		typedef ElemCritEntry<U> ElemCritEntryType;
		typedef std::set<ElemCritEntryType, CompareEntry, ALLOCC> ElemCritEntryTypeSet;
		typedef ElemCritDesc<U, CompareEntry, ALLOCC> ElemCritDescType;
	public:
		U  value;
		ElemCritEntryTypeSet entries;
	public:
		ElemCritDesc(const U ival = 0, const U jval = 0, const DistanceMode m = DistanceMode::modeCenter) :
			value(0) {
			if (ival != jval) {
				if (ival < jval) {
					this->value = ival;
					this->entries.insert(ElemCritEntryType(jval, m));
				}
				else {
					this->value = jval;
					this->entries.insert(ElemCritEntryType(ival, m));
				}
			}// add
		}
		ElemCritDesc(const U ival, const ElemCritEntryType &c) : value(ival) {
			this->add(ival, c.first, c.second);
		}
		ElemCritDesc(const ElemCritDescType &other) :value(other.value), entries(other.entries) {}
		ElemCritDescType & operator=(const ElemCritDescType &other) {
			if (this != &other) {
				this->value = other.value;
				this->entries = other.entries;
			}
			return (*this);
		}
		~ElemCritDesc() {}
	public:
		bool contains_index(const U ival) const {
			if (this->value == ival) {
				return (true);
			}
			auto iend = this->entries.end();
			for (auto it = this->entries.begin(); it != iend; ++it) {
				const U x = (*it).first;
				if (x == ival) {
					return (true);
				}
			}// it
			return (false);
		}// contains_index
		bool add(const U ival, const U jval, const DistanceMode m = DistanceMode::modeCenter) {
			bool bRet = false;
			if (ival == jval) {
				return (bRet);
			}
			U i1 = ival, i2 = jval;
			if (i1 > i2) {
				const U t = i1;
				i1 = i2;
				i2 = t;
			}
			if (this->entries.empty()) {
				this->value = i1;
				this->entries.insert(ElemCritEntryType(i2, m));
				bRet = true;
			}
			else if (this->value == i1) {
				if (!this->contains_index(i2)) {
					this->entries.insert(ElemCritEntryType(i2, m));
					bRet = true;
				}
			}
			return (bRet);
		}// add
	};// class ElemCritDesc<U,CompareEntry,ALLOCC>
	  ///////////////////////////////////////
	template <typename Z = long, typename  U = int, class CompareU = std::less<U>,
		class CompareEntry = std::less<ElemCritEntry<U> >,
		class ALLOCU = std::allocator<U>,
		class ALLOCC = std::allocator<ElemCritEntry<U> >,
		class ALLOCP = std::allocator<std::pair<U, ElemCritDesc<U, CompareEntry, ALLOCC>  > >
	> class Crits {
	public:
		typedef ElemCritEntry<U> ElemCritEntryType;
		typedef ElemCritDesc<U, CompareEntry, ALLOCC> ElemCritDescType;
		typedef Crits<Z, U, CompareU, CompareEntry, ALLOCU, ALLOCC, ALLOCP> CritsType;
		//
		typedef std::set<U, CompareU, ALLOCU> IndexTypeSet;
		typedef std::set<ElemCritEntryType, CompareEntry, ALLOCC> ElemCritEntryTypeSet;
		typedef std::map<U, ElemCritDescType, CompareU, ALLOCP> ElemCritDescTypeMap;
	private:
		Z criteria;
		ElemCritDescTypeMap descs;
	public:
		Crits() :criteria(0) {}
		Crits(const CritsType &other) :criteria(other.criteria), descs(other.descs) {}
		CritsType & operator=(const CritsType &other) {
			if (this != &other) {
				this->criteria = other.criteria;
				this->descs = other.descs;
			}
			return (*this);
		}
		~Crits() {}
	public:
		bool empty(void) const {
			return (this->descs.empty());
		}// empty
		void clear(void) {
			this->descs.clear();
		}
		bool add(const Z crit, const U ival, const U jval, const DistanceMode m = DistanceMode::modeCenter) {
			if (ival == jval) {
				return (false);
			}
			U i1 = ival, i2 = jval;
			if (i1 > i2) {
				const U t = i1;
				i1 = i2;
				i2 = t;
			}
			ElemCritDescTypeMap & oMap = this->descs;
			ElemCritDescType oCur(i1, i2, m);
			if (oMap.empty()) {
				this->criteria = crit;
				oMap[i1] = oCur;
				return (true);
			}// empty
			if (crit > this->criteria) {
				return (false);
			}
			if (crit < this->criteria) {
				this->criteria = crit;
				oMap.clear();
				oMap[i1] = oCur;
				return (true);
			}
			if (oMap.find(i1) != oMap.end()) {
				ElemCritDescType &x = oMap[i1];
				return (x.add(i1, i2, m));
			}
			else {
				auto iend = oMap.end();
				for (auto it = oMap.begin(); it != iend; ++it) {
					ElemCritDescType &x = (*it).second;
					if (x.contains_index(i2)) {
						return (false);
					}
				}// it
				oMap[i1] = oCur;
			}
			return	(true);
		}// add
		void keys(IndexTypeSet &oSet) const {
			oSet.clear();
			const ElemCritDescTypeMap & oMap = this->descs;
			auto iend = oMap.end();
			for (auto it = oMap.begin(); it != iend; ++it) {
				oSet.insert((*it).first);
			}
		}// keys
		bool entry_value(const U key, IndexTypeSet &oSet) const {
			oSet.clear();
			const ElemCritDescTypeMap & oMap = this->descs;
			auto it = oMap.find(key);
			if (it != oMap.end()) {
				const ElemCritDescType &x = (*it).second;
				const ElemCritEntryTypeSet vals = x.entries;
				auto jEnd = vals.end();
				for (auto jt = vals.begin(); jt != jEnd; ++jt) {
					const ElemCritEntryType &c = *jt;
					oSet.insert(c.first);
				}// jt
			}// found
			return (!oSet.empty());
		}// entry_value
		void entry_values(const U key, ElemCritEntryTypeSet &vals) const {
			const ElemCritDescTypeMap & oMap = this->descs;
			auto it = oMap.find(key);
			if (it != oMap.end()) {
				const ElemCritDescType &x = (*it).second;
				vals = x.entries;
			}
			else {
				vals.clear();
			}
		}// entry_values
		bool entry_value(const U key, U &jval, DistanceMode &mode) const {
			mode = DistanceMode::modeInvalid;
			const ElemCritDescTypeMap & oMap = this->descs;
			auto it = oMap.find(key);
			if (it != oMap.end()) {
				const ElemCritDescType &x = (*it).second;
				const ElemCritEntryTypeSet vals = x.entries;
				if (!vals.empty()) {
					auto jt = vals.begin();
					assert(jt != vals.end());
					const ElemCritEntryType &c = *jt;
					jval = c.first;
					mode = c.second;
					return (true);
				}// not empty
			}// found
			return (false);
		}// entry_value
	};// class Crits<Z,U,CompareU,CompareEntry,ALLOCC,ALLOCP>
	////////////////////////////////////
}// namespace info
////////////////////////////////////////
#endif // !__CRITS_H__
