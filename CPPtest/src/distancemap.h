#pragma once
#ifndef __DISTANCEMAP_H__
#define __DISTANCEMAP_H__
/////////////////////////////////
#include "infobase.h"
////////////////////////////////////
namespace info {
///////////////////////////////////////
template<typename U = int, typename Z = long, class COMPAREU = std::less<U>,
		class ALLOCPM1 = std::allocator<std::pair<U, Z> >,
		class ALLOCPM2 = std::allocator<
				std::pair<U, std::map<U, Z, COMPAREU, ALLOCPM1>>> >
class DistanceMap: public BaseInfo {
public:
	typedef std::map<U, Z, COMPAREU, ALLOCPM1> IndexDistanceMapType;
	typedef std::map<U, IndexDistanceMapType, COMPAREU, ALLOCPM2> MyMapType;
	typedef DistanceMap<U, Z, COMPAREU, ALLOCPM1, ALLOCPM2> DistanceMapType;
private:
	MyMapType oMap;
public:
	DistanceMap() {
	}
	DistanceMap(DistanceMapType &other) :
			BaseInfo(other), oMap(other.oMap) {
	}
	DistanceMapType & operator=(const DistanceMapType &other) {
		if (this != &other) {
			BaseInfo::operator=(other);
			this->oMap = other.oMap;
		}
		return (*this);
	}
	virtual ~DistanceMap() {
	}
public:
	template <class COMPAREUX,class ALLOCXU>
	void keys(std::set<U,COMPAREUX,ALLOCXU> &oSet)const {
		oSet.clear();
		for (auto it = this->oMap.begin(); it != this->oMap.end(); ++it){
			oSet.insert((*it).first);
		}// it
	}// keys
	void clear(void) {
		this->oMap.clear();
	}
	Z operator()(const size_t i1, const size_t i2) const {
		Z res = 0;
		this->find(i1, i2, res);
		return (res);
	}
	void add(const U index1, const U index2, const Z res) {
		assert(index1 != index2);
		assert(res > 0);
		U i1 = index1, i2 = index2;
		if (i1 > i2) {
			const U t = i1;
			i1 = i2;
			i2 = t;
		}
		auto it1 = oMap.find(i1);
		if (it1 == oMap.end()) {
			IndexDistanceMapType m;
			m[i2] = res;
			oMap[i1] = m;
		} else {
			IndexDistanceMapType &m = (*it1).second;
			m[i2] = res;
		}
	} // add
	bool find(const U index1, const U index2, Z &res) const {
		assert(index1 != index2);
		U i1 = index1, i2 = index2;
		if (i1 > i2) {
			const U t = i1;
			i1 = i2;
			i2 = t;
		}
		auto it1 = oMap.find(i1);
		if (it1 == oMap.end()) {
			res = 0;
			return (false);
		}
		const IndexDistanceMapType &m = (*it1).second;
		auto jt = m.find(i2);
		if (jt != m.end()) {
			res = (*jt).second;
			return (true);
		}
		res = 0;
		return (false);
	} // find
};
// distanceMap;
///////////////////////////////////////////
}// namespace info
//////////////////////////////////
#endif // !__DISTANCEMAP_H__
