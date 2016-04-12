#pragma once
#ifndef __INDEXEDCLUSTER_H__
#define __INDEXEDCLUSTER_H__
////////////////////////////////
#include "distance.h"
#include "distancemap.h"
////////////////////////////////
namespace info {
	////////////////////////////////////
	template <typename U = int, typename Z = long,
		class COMPAREU = std::less<U>,
		class ALLOCU = std::allocator<U>,
		class ALLOCPM1 = std::allocator<std::pair<U, Z> >,
		class ALLOCPM2 = std::allocator<std::pair<U, std::map<U, Z, COMPAREU, ALLOCPM1> >>
	>
	class IndexedCluster : public BaseInfo {
	public:
		typedef U IndexType;
		//
		typedef std::deque<U, ALLOCU> IndexTypeVector;
		typedef std::set<U, COMPAREU, ALLOCU> IndexTypeSet;
		typedef DistanceMap<U, Z, COMPAREU, ALLOCPM1, ALLOCPM2> DistanceMapType;
		typedef IndexedCluster<U, Z, COMPAREU, ALLOCU, ALLOCPM1, ALLOCPM2> IndexedClusterType;
	protected:
		const DistanceMapType *_pmap;
		IndexTypeVector _members;
	public:
		IndexedCluster(const DistanceMapType *pStore) :_pmap(pStore) {}
		IndexedCluster(const DistanceMapType *pStore, const IndexType aIndex) :
			_pmap(pStore) {
			this->_members.push_back(aIndex);
		}
		IndexedCluster(const IndexedCluster &other) :BaseInfo(other),
			_pmap(other._pmap), _members(other._members) {}
		IndexedCluster & operator=(const IndexedCluster &other) {
			if (this != &other) {
				BaseInfo::operator=(other);
				this->_pmap = other._pmap;
				this->_members = other._members;
			}
			return (*this);
		}
		virtual ~IndexedCluster() {}
	public:
		virtual bool is_valid(void) const {
			return (this->_pmap != nullptr);
		}
		bool empty(void) const {
			return (this->_members.empty());
		}
		void clear_members(void) {
			this->_members.clear();
		}// clear_members
		const IndexTypeVector & members(void) const {
			return (this->_members);
		}
		void merge(const IndexedCluster &other, 
			DistanceMode mode = DistanceMode::modeUpUp) {
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
				}// i
				break;
			case DistanceMode::modeUpDown:
				for (size_t i = (size_t)(n - 1); i >= 0; --i) {
					vdest.push_front(vsrc[i]);
					if (i < 1) {
						break;
					}
				}// i
				break;
			case DistanceMode::modeDownUp:
				for (size_t i = 0; i < n; ++i) {
					vdest.push_back(vsrc[i]);
				}// i
				break;
			case DistanceMode::modeDownDown:
				for (size_t i = (size_t)(n - 1); i >= 0; --i) {
					vdest.push_back(vsrc[i]);
					if (i < 1) {
						break;
					}
				}// i
				break;
			default:
				// should not be here
				assert(false);
				break;
			}// mode
		}// merge
		bool arrange_distance(const IndexedCluster &other,
			std::pair<Z, DistanceMode> &res) const {
			if (!this->is_valid()) {
				return (false);
			}
			const IndexTypeVector & vv1 = this->_members;
			const IndexTypeVector & vv2 = other._members;
			const size_t n1 = vv1.size();
			const size_t n2 = vv2.size();
			res.second = DistanceMode::modeUpUp;
			res.first = 0;
			if ((n1 < 1) || (n2 < 1)) {
				return (false);
			}
			const DistanceMapType &oDist = *(this->_pmap);
			if ((n1 == 1) && (n2 == 1)) {
				oDist.find(vv1[0], vv2[0], res.first);
			}
			else if ((n1 == 1) && (n2 > 1)) {
				oDist.find(vv1[0], vv2[0], res.first);
				Z dd = 0;
				oDist.find(vv1[0], vv2[n2 - 1], dd);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeUpDown;
				}
			}
			else if ((n1 > 1) && (n2 == 1)) {
				oDist.find(vv1[0], vv2[0], res.first);
				Z dd = 0;
				oDist.find(vv1[n1 - 1], vv2[0], dd);
				if (dd < res.first) {
					res.first = dd;
					res.second = DistanceMode::modeDownUp;
				}
			}
			else {
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
			return	(true);
		}// arrange_distance
	};// class IndexedCluster
	////////////////////////////////////////
}// namespace info
////////////////////////////////
#endif // !__INDEXEDCLUSTER_H__
