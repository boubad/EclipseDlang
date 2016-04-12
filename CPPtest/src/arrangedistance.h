/*
 * arrangedistance.h
 *
 *  Created on: 12 avr. 2016
 *      Author: boubad
 */

#ifndef ARRANGEDISTANCE_H_
#define ARRANGEDISTANCE_H_
/////////////////////////////
#include "indexedcluster.h"
/////////////////////////////////
namespace info {
//////////////////////////////
template<typename U = int, typename Z = long, class COMPAREU = std::less<U>,
		class ALLOCU = std::allocator<U>, class ALLOCPM1 = std::allocator<
				std::pair<U, Z> >, class ALLOCPM2 = std::allocator<
				std::pair<U, std::map<U, Z, COMPAREU, ALLOCPM1>>>,
		class ALLOCPM3 = std::allocator<
		IndexedCluster<U, Z, COMPAREU, ALLOCU, ALLOCPM1, ALLOCPM2> > >
		class ArrangeDistance: public BaseInfo {
		public:
			typedef U IndexType;
			//
		typedef std::deque<U, ALLOCU> IndexTypeVector;
		typedef std::set<U, COMPAREU, ALLOCU> IndexTypeSet;
		typedef DistanceMap<U, Z, COMPAREU, ALLOCPM1, ALLOCPM2> DistanceMapType;
		typedef IndexedCluster<U, Z, COMPAREU, ALLOCU, ALLOCPM1, ALLOCPM2> IndexedClusterType;
		typedef std::vector<IndexedClusterType,ALLOCPM3> ClusterTypeVector;
		typedef ArrangeDistance<U, Z, COMPAREU, ALLOCU, ALLOCPM1, ALLOCPM2,ALLOCPM3> ArrangeDistanceType;
	protected:
		ClusterTypeVector _clusters;
	public:
		ArrangeDistance(const DistanceMapType *pMap) {
			assert(pMap != nullptr);
			IndexTypeSet oSet;
			pMap->keys(oSet);
			for (auto it = oSet.begin(); it != oSet.end(); ++it) {
				const U aIndex = *it;
				IndexedClusterType c(pMap,aIndex);
				this->_clusters.push_back(c);
			} // it
		} // ArrangeDistance
		ArrangeDistance(const ArrangeDistanceType &other):BaseInfo(other),
		_clusters(other._clusters) {}
		ArrangeDistanceType & operator=(const ArrangeDistanceType &other) {
			if (this != &other) {
				BaseInfo::operator=(other);
				this->_clusters = other._clusters;
			}
			return (*this);
		}
		virtual ~ArrangeDistance() {}

	protected:
		bool find_best_move(size_t &iFirst, size_t &iSecond,
				DistanceMode &mode) const {
			const ClusterTypeVector &vv = this->_clusters;
			const size_t n = vv.size();
			if (n < 2) {
				return (false);
			}
			const IndexedClusterType &c0 = vv[0];
			const IndexedClusterType &c1 = vv[1];
			std::pair<Z, DistanceMode> oPair;
			c0.arrange_distance(c1,oPair);
			iFirst = 0;
			iSecond = 1;
			mode = oPair.second;
			Z dMin = oPair.first;
			for (size_t i = 0; i < n; ++i) {
				const IndexedClusterType &cx1 = vv[i];
				for (size_t j = 0; j < i; ++j) {
					const IndexedClusterType &cx2 = vv[j];
					std::pair<Z, DistanceMode> xPair;
					cx1.arrange_distance(cx2,xPair);
					if (xPair.first < dMin) {
						dMin = xPair.first;
						iFirst = i;
						iSecond = j;
					}
				} // j
			} // i
			return (true);
		} // find_best_move
		bool make_one_step(void) {
			ClusterTypeVector &vv = this->_clusters;
			const size_t n = vv.size();
			if (n < 2) {
				return (false);
			}
			size_t iFirst = 0, iSecond = 0;
			DistanceMode mode = DistanceMode::modeInvalid;
			if (!this->find_best_move(iFirst,iSecond,mode)) {
				return (false);
			}
			assert(iFirst != iSecond);
			IndexedClusterType &c0 = vv[0];
			const IndexedClusterType &c1 = vv[1];
			c0.merge(c1,mode);
			ClusterTypeVector vRes(n - 1);
			size_t ii = 0;
			for (size_t i=0; i < n; ++i) {
				if (i != iSecond) {
					vRes[ii++] = vv[i];
				}
			} // i
			this->_clusters = vRes;
			return (true);
		} //update_one_step
	public:
		const IndexTypeVector & members(void) const {
			const IndexedClusterType &c0 = this->_clusters[0];
			return (c0.members());
		} // members
		void arrange(void) const {
			while (true) {
				ClusterTypeVector &vv = this->_clusters;
				const size_t n = vv.size();
				if (n < 2) {
					break;
				}
				size_t iFirst = 0, iSecond = 0;
				DistanceMode mode = DistanceMode::modeInvalid;
				if (!this->find_best_move(iFirst,iSecond,mode)) {
					break;
				}
				assert(iFirst != iSecond);
				IndexedClusterType &c0 = vv[0];
				const IndexedClusterType &c1 = vv[1];
				c0.merge(c1,mode);
				ClusterTypeVector vRes(n - 1);
				size_t ii = 0;
				for (size_t i=0; i < n; ++i) {
					if (i != iSecond) {
						vRes[ii++] = vv[i];
					}
				} // i
				this->_clusters = vRes;
			} // while
		} // arrange
	}; // class ArrangeDistance<U, Z, COMPAREU, ALLOCU, ALLOCPM1, ALLOCPM2,ALLOCPM3>
//////////////////////////////////
}
// namespace ino
/////////////////////////////////
#endif /* ARRANGEDISTANCE_H_ */
